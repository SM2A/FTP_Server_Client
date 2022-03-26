#ifndef FTP_SERVER_CLIENT_COMMAND
#define FTP_SERVER_CLIENT_COMMAND

#include <string>
#include "User.h"
#include <sstream>

class Command {

private:

    static bool verify(std::string msg, std::string cmd, int count);
    static bool verify(std::string msg, std::string cmd, std::string branch, int count);
    static void response(int fd, int code);
    static void enterCredential(std::string msg, User* user);

    friend class Server;
};

#endif
