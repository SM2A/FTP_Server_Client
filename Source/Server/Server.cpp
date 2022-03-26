#include <fstream>
#include "Server.h"
#include "../Common/json.h"
#include "CommandExecutor.h"
#include "../Common/static.h"
#include "Command.h"

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
        file.close();

        json config = json::parse(data);

        this->dataPort = config["dataChannelPort"];
        this->commandPort = config["commandChannelPort"];

        for (string file_: config["files"]) adminFiles.push_back(file_);
        for (int i = 0; i < config["users"].size(); ++i)
            users.push_back(new User(config["users"][i]["user"], config["users"][i]["password"],
                                     (config["users"][i]["admin"] == "true"),
                                     stoi(string(config["users"][i]["size"])), basePath));
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

                    /*char output[BUFFER] = {0};
                    sprintf(output, "New client with file descriptor %d connected\n", new_socket);
                    write(1, output, strlen(output));

                    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);*/

                    cout << "Command Client Connected " << i << endl;
                    newUsers.push_back(new User("", "", false, 0, ""));
                    newUsers[lastUser]->stage = User::ENTER_USER;
                    newUsers[lastUser]->commandFD = new_socket;
                    lastUser++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        /*char output[BUFFER] = {0};
                        sprintf(output, "Client with file descriptor %d disconnected\n", i);
                        write(1, output, strlen(output));*/

                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    User *user = findUser(i, COMMAND, newUsers);
                    string msg = string(buffer);
                    if (user != nullptr) {
                        cout << "Command Client " << i << " : " << msg << endl;
                        if (user->stage == User::ENTER_USER) {
                            if (Command::verify(msg, "help", 1)) {
                                Command::response(user->commandFD, 214);
                            } else if (Command::verify(msg, "user", 2)) {
                                Command::enterCredential(msg, user);
                                Command::response(user->commandFD, 331);
                                user->stage = User::ENTER_PASSWORD;
                            } else {

                            }
                        } else if (user->stage == User::ENTER_PASSWORD) {
                            if (Command::verify(msg, "help", 1)) {
                                Command::response(user->commandFD, 214);
                            } else if (Command::verify(msg, "pass", 2)) {
                                Command::enterCredential(msg, user);
                                auto loggedInUser = findUser(user->username, user->password, users);
                                if (loggedInUser != nullptr) {
                                    loggedInUser->commandFD = user->commandFD;
                                    loggedInUser->stage = User::LOGGED_IN;
                                    Command::response(user->commandFD, 230);
                                    removeUser(user->commandFD, COMMAND);
                                    lastUser--;
                                } else {
                                    user->stage = User::ENTER_USER;
                                    Command::response(user->commandFD, 430);
                                }
                            } else {

                            }
                        } else if (user->stage == User::LOGGED_IN) {
                            /*if (Command::verify(msg, "help", 1)) {

                            } else if (Command::verify(msg, "user", 2)) {

                            } else {

                            }*/
                        } else {
                            cout << "Command Client " << i << " : " << msg << endl;
                        }
                    }
                }
            }
        }
    }
}

void Server::listenData() {
    fd_set master_set, working_set;

    FD_ZERO(&master_set);
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

                    /*char output[BUFFER] = {0};
                    sprintf(output, "New client with file descriptor %d connected\n", new_socket);
                    write(1, output, strlen(output));

                    sprintf(buffer, "Hello from server, you're client %d\nPlease choose your category :\n"
                                    "1 - Computer\n2 - Electric\n3 - Civil\n4 - Mechanic\n", new_socket);
                    send(new_socket, buffer, strlen(buffer), 0);*/

                    cout << "Data Client Connected " << i << endl;
                    newUsers.push_back(new User("", "", false, 0, ""));
                    newUsers[lastUser]->stage = User::LOGGED_IN;
                    newUsers[lastUser]->dataFD = new_socket;
                    lastUser++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        /*char output[BUFFER] = {0};
                        sprintf(output, "Client with file descriptor %d disconnected\n", i);
                        write(1, output, strlen(output));*/

                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    auto user = findUser(i, DATA, newUsers);
                    string msg = string(buffer);
                    cout << "Data Client " << i << " : " << msg << endl;
                    if (user != nullptr) {
                        if (user->stage == User::LOGGED_IN) {
                            string username, password;
                            stringstream stream(msg);
                            getline(stream, username, ' ');
                            getline(stream, password, ' ');
                            auto dataUser = findUser(username, password, users);
                            if (dataUser != nullptr) dataUser->dataFD = user->dataFD;
                            removeUser(user->dataFD,DATA);
                            lastUser--;
                        } else {
                            cout << "Data Client " << i << " : " << msg << endl;
                        }
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

User *Server::findUser(int fd, fileDescriptor type, vector<User *> _users_) {
    for (auto user : _users_) {
        if ((user->commandFD == fd) && (type == COMMAND)) return user;
        else if ((user->dataFD == fd) && (type == DATA)) return user;
    }
    return nullptr;
}

User *Server::findUser(string username, vector<User *> _users_) {
    for (auto &user : _users_) if (user->username == username) return user;
    return nullptr;
}

User *Server::findUser(string username, string password, vector<User *> _users_) {
    for (auto &user : _users_) if ((user->username == username) && (user->password == password)) return user;
    return nullptr;
}

void Server::removeUser(int fd, fileDescriptor type) {
    for (int i = 0; i < newUsers.size(); ++i) {
        if ((newUsers[i]->commandFD == fd) && (type == COMMAND)) {
            newUsers.erase(newUsers.begin() + i);
            break;
        } else if((newUsers[i]->dataFD == fd) && (type == DATA)){
            newUsers.erase(newUsers.begin() + i);
            break;
        }
    }
}
