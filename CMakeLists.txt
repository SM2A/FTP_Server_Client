cmake_minimum_required(VERSION 3.17)
project(ftp_server_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(server Source/Server/main.cpp Source/Common/static.h Source/Server/CommandExecutor.cpp Source/Server/CommandExecutor.h Source/Server/User.cpp Source/Server/User.h Source/Server/Server.cpp Source/Server/Server.h)
add_executable(client Source/Client/main.cpp Source/Common/static.h)