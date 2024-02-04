cmake_minimum_required(VERSION 3.26)
project(Client_PC)

set(CMAKE_CXX_STANDARD 17)

add_executable(Client_PC main.cpp
        client.h
        client.cpp
        ../Server\ PC/common.cpp)
target_link_libraries(Client_PC wsock32 ws2_32)
target_include_directories(Client_PC PRIVATE "C:/Users/Admin/CLionProjects/Client-Server/Server PC")