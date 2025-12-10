#include "wifiLogin.h"

constexpr const std::string TARGET_SSID = "IIITU_Wireless";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    logFile = fopen("log.txt", "a+");

    // Parse the command line arguments, in the form of tool.exe USERNAME PASSWORD
    // logout if run as tool.exe logout logout
    int argc = 0;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc < 3)
    {
        std::cout << "Launch the tool with 2 arguments. Example: tool.exe USERNAME PASSWORD\n";
        logToFile(
            "Tool launched without enough arguments.",
            LogMsgLevel::LOG_ERROR);
        return -1;
    }

    USERNAME = narrow_ascii(wargv[1]);
    PASSWORD = narrow_ascii(wargv[2]);

    LocalFree(wargv);

    httplib::Client cli("192.168.100.1", 8090);

    if (USERNAME == "logout" && PASSWORD == "logout")
    {
        logout(cli);
        return 0;
    }

    // Windows API magic(?)
    DWORD ver;
    HANDLE handle;

    WlanOpenHandle(2, NULL, &ver, &handle);
    PWLAN_INTERFACE_INFO_LIST ifList;
    WlanEnumInterfaces(handle, NULL, &ifList);

    logToFile(
        "Tool was executed! Let's see if we can do the fun stuff",
        LogMsgLevel::LOG_INFO);

    for (int i = 0; i < ifList->dwNumberOfItems; i++)
    {
        GUID id = ifList->InterfaceInfo[i].InterfaceGuid;

        PWLAN_CONNECTION_ATTRIBUTES attr;
        DWORD outSize;
        WlanQueryInterface(
            handle,
            &id,
            wlan_intf_opcode_current_connection,
            NULL,
            &outSize,
            (PVOID *)&attr,
            NULL);

        DOT11_SSID ssid = attr->wlanAssociationAttributes.dot11Ssid;
        std::string networkSSID(
            reinterpret_cast<const char *>(ssid.ucSSID),
            ssid.uSSIDLength);

        // If we are connected to the target network, do the fun stuff
        if (networkSSID == TARGET_SSID)
        {
            logToFile(
                std::format("We are connected to {}. Let the fun stuff begin", TARGET_SSID).c_str(),
                LogMsgLevel::LOG_INFO);
            funStuff(cli);
        }
        else
        {
            logToFile(
                std::format("We are NOT connected to {}. No fun stuff here, exitting.", TARGET_SSID).c_str(),
                LogMsgLevel::LOG_INFO);
        }

        break;
    }

    return 0;
}

void funStuff(httplib::Client &httpClient)
{

    if (auto res = httpClient.Get("/httpclient.html"))
    {
        logToFile(
            std::format("Login page httpclient.html online. Status: {}", res->status).c_str(),
            LogMsgLevel::LOG_INFO);

        // Send credentials and log in, then start the heartbeat loop
        if (login(httpClient))
        {
            logToFile(
                "Starting heartbeat loop.",
                LogMsgLevel::LOG_INFO);
            startHeartbeat(httpClient);
        }
    }
    else
    {
        auto err = res.error();
        logToFile(
            std::format("HTTP Error while fetching /httpclient.html: {}", httplib::to_string(err)).c_str(),
            LogMsgLevel::LOG_ERROR);
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }
}

bool login(httplib::Client &httpClient)
{
    auto now = std::chrono::system_clock::now();

    auto payload = std::format(
        "mode=191&username={}&password={}&a={}&producttype=0",
        USERNAME,
        PASSWORD,
        std::chrono::system_clock::to_time_t(now) * 100);

    logToFile(
        "Sending credentials to the login endpoint.",
        LogMsgLevel::LOG_INFO);
    logToFile(
        std::format("Login payload: {}", payload).c_str(),
        LogMsgLevel::LOG_INFO);

    auto res = httpClient.Post(
        "/login.xml",
        payload.c_str(),
        "application/x-www-form-urlencoded");

    logToFile(
        std::format("Login response status: {}", res->status).c_str(),
        LogMsgLevel::LOG_INFO);

    logToFile(
        // Log body while removing trailing newline
        std::format("Login response body: {}", res->body.substr(0, res->body.length() - 1)).c_str(),
        LogMsgLevel::LOG_INFO);

    // Check if login succeeded
    if (res->body.find("Login failed") == std::string::npos)
    {
        logToFile(
            "Login succeeded.",
            LogMsgLevel::LOG_INFO);
        return true;
    }
    else
    {
        logToFile(
            "Login failed.",
            LogMsgLevel::LOG_INFO);
        return false;
    }
}

void logout(httplib::Client &httpClient)
{
    auto now = std::chrono::system_clock::now();
    std::time_t epoch_time = std::chrono::system_clock::to_time_t(now);

    auto payload = std::format(
        "mode=193&username={}&a={}&producttype=0",
        USERNAME,
        std::chrono::system_clock::to_time_t(now) * 100);

    logToFile(
        "Calling logout endpoint.",
        LogMsgLevel::LOG_INFO);
    logToFile(
        std::format("Logout payload: {}", payload).c_str(),
        LogMsgLevel::LOG_INFO);

    auto res = httpClient.Post(
        "/logout.xml",
        payload.c_str(),
        "application/x-www-form-urlencoded");

    logToFile(
        std::format("Logout response status: {}", res->status).c_str(),
        LogMsgLevel::LOG_INFO);
}

void startHeartbeat(httplib::Client &httpClient)
{
    while (PROGRAM_RUNNING)
    {
        auto now = std::chrono::system_clock::now();
        auto payload = std::format(
            "mode=192&username={}&a={}&producttype=0",
            USERNAME,
            std::chrono::system_clock::to_time_t(now) * 100);
        auto res = httpClient.Get(
            std::format("/live?{}", payload));
        std::this_thread::sleep_for(std::chrono::minutes(3));
    }
}

void logToFile(const char *message, LogMsgLevel lvl)
{
    using namespace std::chrono;

    char logLvl[8];

    switch (lvl)
    {
    case LOG_ERROR:
        strcpy(logLvl, "ERROR");
        break;
    case LOG_WARNING:
        strcpy(logLvl, "WARN");
        break;
    case LOG_INFO:
        strcpy(logLvl, "INFO");
        break;
    }

    auto now = system_clock::now();
    auto local = floor<seconds>(now);

    auto tz = current_zone();
    zoned_time zt{tz, local};

    auto currTimeStr = std::format("{:%d/%m/%y-%H:%M:%S}", zt);

    fputs(
        std::format(
            "[{}] [{}] - {}\n",
            currTimeStr,
            logLvl,
            message)
            .c_str(),
        logFile);
    fflush(logFile);
}