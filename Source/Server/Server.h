#ifndef FTP_SERVER_CLIENT_SERVER
#define FTP_SERVER_CLIENT_SERVER

#include <string>
#include <vector>
#include "User.h"
#include "Logger.h"

class Server {

private:
    static Server *instance;
    std::vector<User *> users;
    std::vector<User *> newUsers;
    std::vector<std::string> adminFiles;
    int commandPort;
    int dataPort;

    int commandFD;
    int dataFD;
    enum fileDescriptor {COMMAND, DATA};
    std::string basePath;

    int new_socket;
    int lastUser;
    int maxSD;

    Logger *logger;

    Server() {
        lastUser = 0;
        logger = new Logger();
    }

    int acceptClientCommand();
    int acceptClientData();
    User *findUser(int fd, fileDescriptor type, std::vector<User *> _users_);
    User *findUser(std::string username, std::vector<User *> _users_);
    User *findUser(std::string username, std::string password, std::vector<User *> _users_);
    void removeUser(int fd, fileDescriptor type);
    bool canAccess(User *user, std::string name);

public:
    static Server *getInstance();

    void init(std::string path);
    void startServer();
    void listenCommand();
    void listenData();
};

#endif
