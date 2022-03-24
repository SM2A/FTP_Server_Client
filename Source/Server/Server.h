#ifndef FTP_SERVER_CLIENT_SERVER
#define FTP_SERVER_CLIENT_SERVER

#include <string>

class Server {

private:
    static Server *instance;

    Server() {}

public:
    static std::string basePath;

    static Server *getInstance();

};

#endif
