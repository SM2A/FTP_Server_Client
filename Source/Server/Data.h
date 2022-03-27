#ifndef FTP_SERVER_CLIENT_DATA
#define FTP_SERVER_CLIENT_DATA

#include <string>

class Data {

private:
    static void response(int fd, std::string data);

    friend class Server;

};

#endif
