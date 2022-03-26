#include <fstream>
#include "Client.h"
#include "../Common/json.h"
#include "../Common/static.h"


#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include <csignal>

using namespace std;
using json = nlohmann::json;

Client *Client::instance = nullptr;

Client *Client::getInstance() {
    if (instance == nullptr) instance = new Client();
    return instance;
}

void Client::init(string path) {
    ifstream file(path);
    if (file.is_open()) {

        string data, line;
        while (getline(file, line)) data += line;
        file.close();

        json config = json::parse(data);

        this->dataPort = config["dataChannelPort"];
        this->commandPort = config["commandChannelPort"];

    } else throw invalid_argument("File not found");
}

int Client::connectServer(int port) {
    struct sockaddr_in server_address;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    if (connect(fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        //todo error
        write(1, "Error connecting to server\n", strlen("Error connecting to server\n"));
        exit(0);
    }

    return fd;
}

void Client::startCommand() {
    commandFD = connectServer(commandPort);
}

void Client::startData() {
    dataFD = connectServer(dataPort);
}
