#ifndef FTP_SERVER_CLIENT_LOGGER
#define FTP_SERVER_CLIENT_LOGGER

#include <string>

class Logger {

public:
    void log(std::string username, std::string msg);
    void log(std::string username, std::string msg, std::string argument);

private:
    std::string FILE_NAME = "log.txt";

    std::string getDateTime();

};

#endif
