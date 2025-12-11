#include "httplib.h"
#include <Windows.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <format>
#include <iostream>
#include <shellapi.h>
#include <wlanapi.h>
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "Shell32.lib")

FILE* logFile;
bool PROGRAM_RUNNING = true;

std::string USERNAME;
std::string PASSWORD;

enum LogMsgLevel {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

void logToFile(const char* message, LogMsgLevel lvl);
void funStuff(httplib::Client& httpClient);
bool login(httplib::Client& httpClient);
void logout(httplib::Client& httpClient);
void startHeartbeat(httplib::Client& httpClient);

std::string narrow_ascii(const std::wstring& w) {
    std::string s;
    s.reserve(w.size());
    for (wchar_t c : w) {
        s.push_back(static_cast<char>(c));
    }
    return s;
}