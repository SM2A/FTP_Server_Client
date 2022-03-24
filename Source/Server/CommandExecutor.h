#ifndef FTP_SERVER_CLIENT_COMMANDEXECUTOR
#define FTP_SERVER_CLIENT_COMMANDEXECUTOR

#include <string>

class CommandExecutor {

public:
    CommandExecutor() {}

    static std::string pwd();
    static std::string cwd(std::string currentPath, std::string destination);
};


#endif
