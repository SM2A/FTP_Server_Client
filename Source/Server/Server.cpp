#include "Data.h"
#include <fstream>
#include <cstdlib>
#include "Server.h"
#include <unistd.h>
#include <iostream>
#include "Command.h"
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../Common/json.h"
#include "CommandExecutor.h"
#include "../Common/static.h"

using namespace std;
using json = nlohmann::json;

Server *Server::instance = nullptr;

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

                    newUsers.push_back(new User("", "", false, 0, ""));
                    newUsers[lastUser]->stage = User::ENTER_USER;
                    newUsers[lastUser]->commandFD = new_socket;
                    lastUser++;
                    logger->log("FD " + to_string(new_socket), "connected");

                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        close(i);
                        FD_CLR(i, &master_set);
                        auto user = findUser(i, COMMAND, users);
                        if (user != nullptr) {
                            logger->log(user->username, "disconnected");
                            user->stage = User::ENTER_USER;
                        } else {
                            logger->log("FD " + to_string(i), "disconnected");
                            removeUser(i, COMMAND);
                        }
                        continue;
                    }

                    string msg = string(buffer);
                    User *user = findUser(i, COMMAND, newUsers);
                    User *commandUser = findUser(i, COMMAND, users);

                    if (user != nullptr) {
                        if (user->stage == User::ENTER_USER) {
                            if (Command::verify(msg, "help", 1)) {
                                Command::response(user->commandFD, 214);
                                logger->log("FD " + to_string(i), "help");
                            } else if (Command::verify(msg, "user", 2)) {
                                Command::enterCredential(msg, user);
                                Command::response(user->commandFD, 331);
                                user->stage = User::ENTER_PASSWORD;
                                logger->log("FD " + to_string(i), "user");
                            } else if (Command::verify(msg, "quit", 1)) {
                                Command::response(user->commandFD, 221);
                                logger->log("FD " + to_string(i), "quit");
                            } else if (Command::verify(msg, "pass", 2) || Command::verify(msg, "pass", 1)) {
                                Command::response(user->commandFD, 503);
                                logger->log("FD " + to_string(i), "pass");
                            } else if (Command::verify(msg, false)) {
                                Command::response(user->commandFD, 332);
                            } else if (Command::verify(msg, true)) {
                                Command::response(user->commandFD, 332);
                            } else {
                                Command::response(user->commandFD, 500);
                            }
                        } else if (user->stage == User::ENTER_PASSWORD) {
                            if (Command::verify(msg, "help", 1)) {
                                Command::response(user->commandFD, 214);
                                logger->log("FD " + to_string(i), "help");
                            } else if (Command::verify(msg, "quit", 1)) {
                                Command::response(user->commandFD, 221);
                                logger->log("FD " + to_string(i), "quit");
                            } else if (Command::verify(msg, "pass", 2)) {
                                logger->log("FD " + to_string(i), "pass");
                                Command::enterCredential(msg, user);
                                auto loggedInUser = findUser(user->username, user->password, users);
                                if (loggedInUser != nullptr) {
                                    loggedInUser->commandFD = user->commandFD;
                                    loggedInUser->stage = User::LOGGED_IN;
                                    Command::response(user->commandFD, 230);
                                    removeUser(user->commandFD, COMMAND);
                                    logger->log(user->username, "logged in");
                                } else {
                                    user->stage = User::ENTER_USER;
                                    Command::response(user->commandFD, 430);
                                    logger->log("FD " + to_string(i), "wrong username or password");
                                }
                            } else if (Command::verify(msg, false)) {
                                Command::response(user->commandFD, 332);
                            } else if (Command::verify(msg, true)) {
                                Command::response(user->commandFD, 332);
                            } else {
                                Command::response(user->commandFD, 503);
                            }
                        }
                    } else if (commandUser != nullptr) {
                        if (commandUser->stage == User::LOGGED_IN) {
                            if (Command::verify(msg, "help", 1)) {
                                Command::response(commandUser->commandFD, 214);
                                logger->log(commandUser->username, "help");
                            } else if (Command::verify(msg, "pwd", 1)) {
                                Command::response(commandUser->commandFD, 257, "pwd", commandUser->path);
                                logger->log(commandUser->username, "pwd");
                            } else if (Command::verify(msg, "quit", 1)) {
                                Command::response(commandUser->commandFD, 221);
                                commandUser->stage = User::ENTER_USER;
                                logger->log(commandUser->username, "quit");
                            } else if (Command::verify(msg, "cwd", 2)) {
                                string path = CommandExecutor::cwd(commandUser->path, Command::getPath(msg, 2));
                                if (path == ERROR) Command::response(commandUser->commandFD, 500);
                                else {
//                                    if (path.size() < Server::basePath.size()) path = Server::basePath; // If client can't get out of server directory
                                    Command::response(commandUser->commandFD, 250, "cwd", path);
                                    commandUser->path = path;
                                }
                                logger->log(commandUser->username, "cwd");
                            } else if (Command::verify(msg, "cwd", 1)) {
                                Command::response(commandUser->commandFD, 250, "cwd", basePath);
                                commandUser->path = basePath;
                                logger->log(commandUser->username, "cwd");
                            } else if (Command::verify(msg, "mkd", 2)) {
                                string name = Command::getPath(msg, 2);
                                if (CommandExecutor::mkd(commandUser->path, name))
                                    Command::response(commandUser->commandFD, 257, "mkd", name);
                                else Command::response(commandUser->commandFD, 500);
                                logger->log(commandUser->username, "mkd", name);
                            } else if (Command::verify(msg, "dele", "-f", 3)) {
                                string name = Command::getPath(msg, 3);
                                if (canAccess(commandUser, name)) {
                                    if (CommandExecutor::dele(commandUser->path, "-f", name))
                                        Command::response(commandUser->commandFD, 250, "-f", name);
                                    else Command::response(commandUser->commandFD, 500);
                                } else Command::response(commandUser->commandFD, 550);
                                logger->log(commandUser->username, "dele", "-f " + name);
                            } else if (Command::verify(msg, "dele", "-d", 3)) {
                                string name = Command::getPath(msg, 3);
                                if (CommandExecutor::dele(commandUser->path, "-d", name))
                                    Command::response(commandUser->commandFD, 250, "-d", name);
                                else Command::response(commandUser->commandFD, 500);
                                logger->log(commandUser->username, "dele", "-d " + name);
                            } else if (Command::verify(msg, "rename", 3)) {
                                string from = Command::getPath(msg, 2);
                                string to = Command::getPath(msg, 3);
                                if (canAccess(commandUser, from)) {
                                    if (CommandExecutor::rename(commandUser->path, from, to))
                                        Command::response(commandUser->commandFD, 250, "re", "");
                                    else Command::response(commandUser->commandFD, 500);
                                } else Command::response(commandUser->commandFD, 550);
                                logger->log(commandUser->username, "rename", "from " + from + " to " + to);
                            } else if (Command::verify(msg, "ls", 1)) {
                                string ls = CONSOLE;
                                ls += "\n";
                                ls += CommandExecutor::ls(commandUser->path);
                                Data::response(commandUser->dataFD, ls);
                                Command::response(commandUser->commandFD, 226, "ls");
                                logger->log(commandUser->username, "ls");
                            } else if (Command::verify(msg, "retr", 2)) {
                                string name = Command::getPath(msg, 2);
                                if (canAccess(commandUser, name)) {
                                    long size = CommandExecutor::getFileSize(name);
                                    if ((size != -1) && (size < (BUFFER - 200))) {
                                        size /= 1024;
                                        if (commandUser->capacity >= size) {
                                            commandUser->capacity -= size;
                                            string ls = _FILE_;
                                            ls += "\n";
                                            ls += (commandUser->path + "/" + CommandExecutor::getFileName(name));
                                            ls += "\n";
                                            ls += CommandExecutor::getFileContent(name);
                                            Data::response(commandUser->dataFD, ls);
                                            Command::response(commandUser->commandFD, 226, "re");
                                        } else {
                                            Data::response(commandUser->dataFD, ERROR);
                                            Command::response(commandUser->commandFD, 425);
                                        }
                                    } else {
                                        Data::response(commandUser->dataFD, ERROR);
                                        Command::response(commandUser->commandFD, 500);
                                    }
                                } else {
                                    Data::response(commandUser->dataFD, ERROR);
                                    Command::response(commandUser->commandFD, 550);
                                }
                                logger->log(commandUser->username, "retr", name);
                            } else if (Command::verify(msg, "pass", 2)) {
                                Command::response(commandUser->commandFD, 500);
                                logger->log(commandUser->username, "pass");
                            } else if (Command::verify(msg, "user", 2)) {
                                Command::response(commandUser->commandFD, 500);
                                logger->log(commandUser->username, "user");
                            } else if (Command::verify(msg, false)) {
                                Command::response(commandUser->commandFD, 501);
                            } else if (Command::verify(msg, true)) {
                                Command::response(commandUser->commandFD, 501);
                            } else {
                                Command::response(commandUser->commandFD, 500);
                            }
                        }
                    } else {
                        logger->log("FD " + to_string(i), " : ", msg);
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

                    newUsers.push_back(new User("", "", false, 0, ""));
                    newUsers[lastUser]->stage = User::LOGGED_IN;
                    newUsers[lastUser]->dataFD = new_socket;
                    lastUser++;
                } else {
                    int bytes_received;
                    bytes_received = recv(i, buffer, BUFFER, 0);

                    if (bytes_received == 0) {
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }

                    string msg = string(buffer);
                    auto user = findUser(i, DATA, newUsers);
                    if (user != nullptr) {
                        if (user->stage == User::LOGGED_IN) {
                            string username, password;
                            stringstream stream(msg);
                            getline(stream, username, ' ');
                            getline(stream, password, ' ');
                            auto dataUser = findUser(username, password, users);
                            if (dataUser != nullptr) dataUser->dataFD = user->dataFD;
                            removeUser(user->dataFD, DATA);
                        } else {
                            logger->log("FD " + to_string(i), " : ", msg);
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
            lastUser--;
            break;
        } else if ((newUsers[i]->dataFD == fd) && (type == DATA)) {
            newUsers.erase(newUsers.begin() + i);
            lastUser--;
            break;
        }
    }
}

bool Server::canAccess(User *user, string name) {
    if (user->isAdmin) return true;
    string fileName = CommandExecutor::getFileName(name);
    for (string file : adminFiles) if ((!user->isAdmin) && (file == fileName)) return false;
    return true;
}
