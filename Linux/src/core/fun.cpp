//
// Created by devboi on 5/19/26.
//

#include <format>
#include <iostream>
#include "core/fun.hpp"
#include "details.h"

namespace fun {

void funStuff(httplib::Client& httpClient) {

    if (auto res = httpClient.Get("/httpclient.html")) {
        std::cout << std::format("Login page httpclient.html online. Status: {}", res->status) << std::endl;

        // Send credentials and log in, then start the heartbeat loop
        if (login(httpClient)) {
            std::cout << "Starting heartbeat loop." << std::endl;
            startHeartbeat(httpClient);
        }
    } else {
        auto err = res.error();
        std::cout << std::format("HTTP Error while fetching /httpclient.html: {}", httplib::to_string(err)) << std::endl,
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }
}

bool login(httplib::Client& httpClient) {
    auto now = std::chrono::system_clock::now();

    auto payload = std::format(
        "mode=191&username={}&password={}&a={}&producttype=0",
        consts::USERNAME,
        consts::PASSWORD,
        std::chrono::system_clock::to_time_t(now) * 100);

    std::cout << "Sending credentials to the login endpoint." << std::endl;
    std::cout << std::format("Login payload: {}", payload) << std::endl;

    auto res = httpClient.Post(
        "/login.xml",
        payload.c_str(),
        "application/x-www-form-urlencoded");

    std::cout << std::format("Login response status: {}", res->status) << std::endl;

    std::cout <<
        // Log body while removing trailing newline
        std::format("Login response body: {}", res->body.substr(0, res->body.length() - 1))  << std::endl;

    // Check if login succeeded
    if (res->body.find("Login failed") == std::string::npos) {
        std::cout << "Login succeeded." << std::endl;
        return true;
    } else {
        std::cout << "Login failed." << std::endl;
        return false;
    }
}

void logout(httplib::Client& httpClient) {
    auto now = std::chrono::system_clock::now();
    std::time_t epoch_time = std::chrono::system_clock::to_time_t(now);

    auto payload = std::format(
        "mode=193&username={}&a={}&producttype=0",
        consts::USERNAME,
        std::chrono::system_clock::to_time_t(now) * 100);

    std::cout << "Calling logout endpoint." << std::endl;
    std::cout << std::format("Logout payload: {}", payload) << std::endl;

    auto res = httpClient.Post(
        "/logout.xml",
        payload.c_str(),
        "application/x-www-form-urlencoded");

    std::cout << std::format("Logout response status: {}", res->status) << std::endl;
}

void startHeartbeat(httplib::Client& httpClient) {
    while (true) {
        auto now = std::chrono::system_clock::now();
        auto payload = std::format(
            "mode=192&username={}&a={}&producttype=0",
            consts::USERNAME,
            std::chrono::system_clock::to_time_t(now) * 100);
        auto res = httpClient.Get(
            std::format("/live?{}", payload));
        std::this_thread::sleep_for(std::chrono::seconds(60 * 2 + 50)); // 2 Minutes 50 Seconds
    }
}


}