#ifndef FTP_SERVER_CLIENT_COMMANDEXECUTOR
#define FTP_SERVER_CLIENT_COMMANDEXECUTOR

#include <string>

class CommandExecutor {

public:
    static std::string pwd();
    static std::string cwd(std::string currentPath, std::string destination);
    static bool mkd(std::string currentPath, std::string path);
    static bool dele(std::string currentPath, std::string branch, std::string name);
    static std::string ls(std::string currentPath);
    static bool rename(std::string currentPath, std::string currentName, std::string afterName);
    static std::string help();
    static std::string getFileName(std::string path);
    static long getFileSize(std::string path);
    static std::string getFileContent(std::string path);
};


#endif
