#ifndef FTP_SERVER_CLIENT_SERVER
#define FTP_SERVER_CLIENT_SERVER

#include <map>
#include <string>
#include <vector>
#include "User.h"

class Server {

private:
    static Server *instance;
    std::map<int, User *> users;
    std::vector<std::string> files;
    int commandPort;
    int dataPort;

    int commandFD;
    int dataFD;

    int emptyID;

    Server() {emptyID = 0;}

public:
    static std::string basePath;
    static Server *getInstance();

    void init(std::string path);
    void startServer();

    int getCommandPort() { return commandPort;};
    int getDataPort() { return dataPort;};
};

#endif
