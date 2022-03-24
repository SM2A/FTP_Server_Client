#include <unistd.h>
#include <sys/stat.h>
#include "CommandExecutor.h"

using namespace std;

string CommandExecutor::pwd() {
    char path[1000];
    getcwd(path, 1000);
    return string(path);
}

string CommandExecutor::cwd(string currentPath, string destination) {
    if (destination == "..") {
        for (int i = currentPath.size(); i != 0; i--) {
            if (currentPath[i] == '/') {
                currentPath.erase(i);
                break;
            }
            currentPath.erase(i);
        }
        return currentPath;
    }

    struct stat buffer;
    if (stat(destination.c_str(), &buffer) == 0)return destination;
    else throw exception();
}
