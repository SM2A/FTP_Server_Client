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
            emptyID--;
            users.insert({emptyID, new User(config["users"][i]["user"], config["users"][i]["password"],
                                            (config["users"][i]["admin"] == "true"),
                                            stoi(string(config["users"][i]["size"])), basePath)});
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

    cout << commandFD << endl;
    cout << dataFD << endl;
}
