cmake_minimum_required(VERSION 3.17)
project(ftp_server_client)

set(CMAKE_CXX_STANDARD 11)

add_executable(server Source/Server/main.cpp Source/Common/static.h Source/Server/CommandExecutor.cpp Source/Server/CommandExecutor.h Source/Server/User.cpp Source/Server/User.h Source/Server/Server.cpp Source/Server/Server.h Source/Common/json.h Source/Server/Logger.cpp Source/Server/Logger.h Source/Server/Command.cpp Source/Server/Command.h)
add_executable(client Source/Client/main.cpp Source/Common/static.h Source/Client/Client.cpp Source/Client/Client.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
#target_link_libraries(client PRIVATE Threads::Threads)
