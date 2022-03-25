#include "User.h"

User::User(std::string _username_, std::string _password_, bool _isAdmin_, long _capacity_, std::string _path_) {
    this->username = _username_;
    this->password = _password_;
    this->isAdmin = _isAdmin_;
    this->capacity = _capacity_;
    this->path = _path_;
    this->stage = LOGGED_OUT;
    this->commandFD = -1;
    this->dataFD = -1;
}

int User::getCommandFd() const {
    return commandFD;
}

void User::setCommandFd(int commandFd) {
    commandFD = commandFd;
}

int User::getDataFd() const {
    return dataFD;
}

void User::setDataFd(int dataFd) {
    dataFD = dataFd;
}
