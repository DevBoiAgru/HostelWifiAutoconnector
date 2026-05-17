cmake_minimum_required(VERSION 4.2)
project(HostelWifiConnectorLinux)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/thirdparty)

add_executable(HostelWifiConnectorLinux
        include/thirdparty/httplib.h
        include/details.h
        src/main.cpp)
