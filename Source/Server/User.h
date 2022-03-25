#ifndef FTP_SERVER_CLIENT_USER_H
#define FTP_SERVER_CLIENT_USER_H

#include <string>

class User {

private:
    std::string username;
    std::string password;
    bool isAdmin;
    long capacity;
    std::string path;
    int stage;

public:
    enum stage {ENTER_USER, ENTER_PASSWORD, LOGGED_IN, LOGGED_OUT};

    User(std::string _username_, std::string _password_, bool _isAdmin_, long _capacity_, std::string _path_);
};

#endif
