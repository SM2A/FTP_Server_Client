#ifndef FTP_SERVER_CLIENT_SERVER
#define FTP_SERVER_CLIENT_SERVER

#include <map>
#include <string>
#include <vector>
#include "User.h"

class Server {

private:
    static Server *instance;
    std::vector<User*> users;
    std::vector<std::string> adminFiles;
    int commandPort;
    int dataPort;

    int commandFD;
    int dataFD;

    int lastUser;
    int maxSD;

    Server() {lastUser = 0;}

public:
    static std::string basePath;
    static Server *getInstance();

    void init(std::string path);
    void startServer();
    void listenCommand();
    void listenData();
    int acceptClientCommand();
    int acceptClientData();

    int getCommandPort() { return commandPort;};
    int getDataPort() { return dataPort;};
};

#endif
