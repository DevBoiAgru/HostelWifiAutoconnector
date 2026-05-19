#include "core/detect.hpp"
#include "core/fun.hpp"
#include "details.h"
#include "thirdparty/httplib.h"
#include <cstdlib>
#include <fstream>

int main(const int argc, char* argv[]) {
    // We get information about the event and network in the command line arguments from Network Manager
    // check 'man NetworkManager-dispatcher' for more info
    // Additional info is stored in the environment variables

    if (argc < 3)
        return 0;

    static std::ofstream log(consts::LOG_FILE_PATH, std::ios::trunc);

    std::cout.rdbuf(log.rdbuf());
    std::cerr.rdbuf(log.rdbuf());

    std::string action = argv[2];
    if (action != "up") {
        // We are only interested in 'up' events
        return 0;
    }

    char* networkName_cstr = std::getenv("CONNECTION_ID");

    if (networkName_cstr == nullptr) {
        std::cout << "CONNECTION_ID environment variable returned NULL." << std::endl;
        return 1;
    }

    std::string networkName = std::string(networkName_cstr);

    std::cout << "Connected to: " << std::string(networkName) << std::endl;

    httplib::Client portalClient(consts::CAPTIVE_PORTAL_IP, consts::CAPTIVE_PORTAL_PORT);

    std::cout << "Checking for captive portal." << std::endl;
    try {
        bool portal = utils::detectPortal();

        if (portal) {
            std::cout << "Portal found." << std::endl;

            // Start fun stuff
            fun::funStuff(portalClient);

        }   else {
            std::cout << "No portal found." << std::endl;
        }
    } catch (std::exception& e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
}