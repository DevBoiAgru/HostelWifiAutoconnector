//
// Created by devboi on 5/17/26.
//

#ifndef LINUX_CAPTIVEDETAILS_H
#define LINUX_CAPTIVEDETAILS_H

#include <string>

namespace consts {
// Enter your details here
// USERNAME is the username for the Wi-Fi, which should be your roll number
// PASSWORD is the password, which should be your roll number if you didn't change it
const std::string USERNAME = "25ABCD1234";
const std::string PASSWORD = "PASSWORD";

const std::string WIFI_NAME = "IIITU_Wireless";     // Wi-Fi name to execute on, case-insensitive

const std::string CAPTIVE_PORTAL_IP = "192.168.100.1";
constexpr int CAPTIVE_PORTAL_PORT = 8090;

// The URL to detect network, using a /generate_204
const std::string PORTAL_DETECT_URL = "http://connectivitycheck.gstatic.com";

//const std::string LOG_FILE_PATH = "/var/log/wifi_autoconnector.log";
const std::string LOG_FILE_PATH = "/home/devboi/wifi_autoconnector.log";

}

#endif //LINUX_CAPTIVEDETAILS_H
