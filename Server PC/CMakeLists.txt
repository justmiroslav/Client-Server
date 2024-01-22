cmake_minimum_required(VERSION 3.26)
project(Server_PC)

set(CMAKE_CXX_STANDARD 17)

add_executable(Server_PC main.cpp
        common.h
        server.h)
target_link_libraries(Server_PC wsock32 ws2_32)