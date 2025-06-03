cmake_minimum_required(VERSION 3.10)
project(QuickChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(quickchat
    src/main.cpp
    src/server/Server.cpp
    src/client/Client.cpp
)

target_include_directories(quickchat PRIVATE src)
