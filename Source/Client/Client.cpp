#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../Common/json.h"
#include "../Common/static.h"

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

    if (connect(fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        throw runtime_error("Error connecting to server");

    return fd;
}

void Client::startCommand() {
    commandFD = connectServer(commandPort);
}

void Client::startData() {
    dataFD = connectServer(dataPort);
    string input = username + " " + password;
    send(dataFD, input.c_str(), input.size(), 0);
}

void Client::sendCommand() {
    string input;
    getline(cin, input);
    send(commandFD, input.c_str(), input.size(), 0);

    string cmd, value;
    stringstream stream(input);
    getline(stream, cmd, ' ');
    if (getline(stream, value, ' ')) {
        if (cmd == "user") username = value;
        else if (cmd == "pass") password = value;
    }

    if (cmd == "ls") receiveDataResponse();
    if ((cmd == "retr") && (!value.empty())) receiveDataResponse();
}

void Client::receiveCommandResponse() {
    char buff[BUFFER] = {0};
    recv(commandFD, buff, BUFFER, 0);
    cout << string(buff) << endl;

    if (responseCode(string(buff), 230)) startData();
    if (responseCode(string(buff), 221)) exit(0);
}

bool Client::responseCode(string msg, int code) {
    string _code_;
    stringstream stream(msg);
    getline(stream, _code_, ':');
    if (_code_ == to_string(code)) return true;
    return false;
}

void Client::receiveDataResponse() {
    char buff[BUFFER] = {0};
    recv(dataFD, buff, BUFFER, 0);

    string type;
    stringstream stream((string(buff)));
    getline(stream, type);

    if (type == CONSOLE) {
        string line;
        while (getline(stream, line)) cout << line << endl;
    } else if (type == _FILE_) {
        string line, content, path;
        getline(stream, path);
        while (getline(stream, line, '\n')) content += (line + "\n");
        ofstream file(path);
        file << content;
        file.close();
    }

    receiveCommandResponse();
    sendCommand();
}
