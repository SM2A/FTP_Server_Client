#include "Server.h"

using namespace std;

Server *Server::instance = nullptr;
string Server::basePath;

Server *Server::getInstance() {
    if (instance == nullptr) instance = new Server();
    return instance;
}
