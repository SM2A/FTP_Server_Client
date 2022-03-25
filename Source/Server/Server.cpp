#include <fstream>
#include "Server.h"
#include "../Common/json.h"
#include "CommandExecutor.h"

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
            users.insert({((-1) * (i + 1)), new User(config["users"][i]["user"], config["users"][i]["password"],
                                                     (config["users"][i]["admin"] == "true"),
                                                     stoi(string(config["users"][i]["size"])), basePath)});
            emptyID--;
        }
        file.close();
    } else throw invalid_argument("File not found");
}
