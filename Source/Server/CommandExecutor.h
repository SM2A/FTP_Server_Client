#ifndef FTP_SERVER_CLIENT_COMMANDEXECUTOR
#define FTP_SERVER_CLIENT_COMMANDEXECUTOR

#include <string>

class CommandExecutor {

public:
    CommandExecutor() {}

    static std::string pwd();
    static std::string cwd(std::string currentPath, std::string destination);
    static bool mkd(std::string currentPath, std::string path);
    static bool dele(std::string currentPath, std::string branch, std::string name);
};


#endif
