#include <fstream>
#include "Server.h"
#include "../Common/json.h"
#include "CommandExecutor.h"
#include "../Common/static.h"

#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>

using namespace std;
using json = nlohmann::json;

Server *Server::instance = nullptr;
string Server::basePath;

Server *Server::getInstance() {
    if (instance == nullptr) instance = new Server();
    return instance;
}

void Server::init(string path) {
    basePath = CommandExecutor::pwd();
    ifstream file(path);
    if (file.is_open()) {

        string data, line;
        while (getline(file, line)) data += line;

        json config = json::parse(data);

        this->dataPort = config["dataChannelPort"];
        this->commandPort = config["commandChannelPort"];

        for (int i = 0; i < config["files"].size(); ++i) files.push_back(config["files"][i]);
        for (int i = 0; i < config["users"].size(); ++i) {
            users.push_back(new User(config["users"][i]["user"], config["users"][i]["password"],
                                            (config["users"][i]["admin"] == "true"),
                                            stoi(string(config["users"][i]["size"])), basePath));
        }
        file.close();
    } else throw invalid_argument("File not found");
}

void Server::startServer() {
    struct sockaddr_in addressCommand;
    commandFD = socket(AF_INET, SOCK_STREAM, 0);

    int optCommand = 1;
    setsockopt(commandFD, SOL_SOCKET, SO_REUSEADDR, &optCommand, sizeof(optCommand));
    addressCommand.sin_family = AF_INET;
    addressCommand.sin_addr.s_addr = INADDR_ANY;
    addressCommand.sin_port = htons(commandPort);

    bind(commandFD, (struct sockaddr *) &addressCommand, sizeof(addressCommand));
    listen(commandFD, 10);

    struct sockaddr_in addressData;
    dataFD = socket(AF_INET, SOCK_STREAM, 0);

    int optData = 1;
    setsockopt(dataFD, SOL_SOCKET, SO_REUSEADDR, &optData, sizeof(optData));
    addressData.sin_family = AF_INET;
    addressData.sin_addr.s_addr = INADDR_ANY;
    addressData.sin_port = htons(dataPort);

    bind(dataFD, (struct sockaddr *) &addressData, sizeof(addressData));
    listen(dataFD, 10);
}

void Server::listenCommand() {

    fd_set master_set, working_set;
    int new_socket;

    FD_ZERO(&master_set);
    maxSD = dataFD;
    FD_SET(commandFD, &master_set);

    while (true) {
        working_set = master_set;
        select(maxSD + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= maxSD; i++) {
            if (FD_ISSET(i, &working_set)) {
                char buffer[BUFFER] = {0};
                if (i == commandFD) {
                    new_socket = acceptClientCommand();
                    FD_SET(new_socket, &master_set);
                    if (new_socket > maxSD) maxSD = new_socket;

                    char output[BUFFER] = {0};
                    sprintf(output, "New client with file descriptor %d connected\n", new_socket);
                    write(1, output, strlen(output));

                    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);

                    users[last_user].stage = CHOOSE_GROUP;
                    users[last_user].fd = new_socket;
                    lastUser++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client with file descriptor %d disconnected\n", i);
                        write(1, output, strlen(output));

                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    if (users[find_user(i)].stage == CHOOSE_GROUP) {
                        int gp = choose_group(buffer);
                        int gp_s = add_to_group(i, gp);
                        users[find_user(i)].stage = WAITING_START;
                        users[find_user(i)].group_category = gp;
                        if (gp_s) start_group(gp);
                    } else if (users[find_user(i)].stage == WAITING_START) {
                        sprintf(buffer, "Please wait ...\n");
                        send(i, buffer, strlen(buffer), 0);
                    } else if (users[find_user(i)].stage == IN_CHAT) {
                        int qa_file = open("QA.txt", O_APPEND | O_RDWR);
                        if (qa_file < 0) qa_file = open("QA.txt", O_CREAT | O_RDWR);
                        write(qa_file, buffer, strlen(buffer));
                        close(qa_file);
                    } else {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client %d : %s", i, buffer);
                        write(1, output, strlen(output));
                    }
                }
            }
        }
    }
}

void Server::listenData() {
    fd_set master_set, working_set;
    int new_socket;

    FD_ZERO(&master_set);
    maxSD = dataFD;
    FD_SET(dataFD, &master_set);

    while (true) {
        working_set = master_set;
        select(maxSD + 1, &working_set, NULL, NULL, NULL);

        for (int i = 0; i <= maxSD; i++) {
            if (FD_ISSET(i, &working_set)) {
                char buffer[BUFFER] = {0};
                if (i == dataFD) {
                    new_socket = acceptClientData();
                    FD_SET(new_socket, &master_set);
                    if (new_socket > maxSD) maxSD = new_socket;

                    char output[BUFFER] = {0};
                    sprintf(output, "New client with file descriptor %d connected\n", new_socket);
                    write(1, output, strlen(output));

                    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);

                    users[last_user].stage = CHOOSE_GROUP;
                    users[last_user].fd = new_socket;
                    lastUser++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client with file descriptor %d disconnected\n", i);
                        write(1, output, strlen(output));

                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    if (users[find_user(i)].stage == CHOOSE_GROUP) {
                        int gp = choose_group(buffer);
                        int gp_s = add_to_group(i, gp);
                        users[find_user(i)].stage = WAITING_START;
                        users[find_user(i)].group_category = gp;
                        if (gp_s) start_group(gp);
                    } else if (users[find_user(i)].stage == WAITING_START) {
                        sprintf(buffer, "Please wait ...\n");
                        send(i, buffer, strlen(buffer), 0);
                    } else if (users[find_user(i)].stage == IN_CHAT) {
                        int qa_file = open("QA.txt", O_APPEND | O_RDWR);
                        if (qa_file < 0) qa_file = open("QA.txt", O_CREAT | O_RDWR);
                        write(qa_file, buffer, strlen(buffer));
                        close(qa_file);
                    } else {
                        char output[BUFFER] = {0};
                        sprintf(output, "Client %d : %s", i, buffer);
                        write(1, output, strlen(output));
                    }
                }
            }
        }
    }
}

int Server::acceptClientCommand() {
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(commandFD, (struct sockaddr *) &client_address, (socklen_t *) &address_len);
    return client_fd;
}

int Server::acceptClientData() {
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    int client_fd = accept(dataFD, (struct sockaddr *) &client_address, (socklen_t *) &address_len);
    return client_fd;
}
