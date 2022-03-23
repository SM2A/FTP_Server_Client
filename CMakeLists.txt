cmake_minimum_required(VERSION 3.17)
project(ftp_server_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(server Source/Server/main.cpp)
add_executable(client Source/Client/main.cpp)