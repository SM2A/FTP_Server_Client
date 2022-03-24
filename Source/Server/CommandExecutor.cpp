#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "CommandExecutor.h"
#include "../Common/static.h"

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
    else return ERROR;
}

bool CommandExecutor::mkd(string currentPath, string path) {
    return (mkdir((currentPath + "/" + path).c_str(), 0777) == 0);
}

bool CommandExecutor::dele(string currentPath, string branch, string name) {

    if (branch == "-f") return (remove((currentPath + "/" + name).c_str()) == 0);
    else if (branch == "-d") {
        if (cwd(currentPath, currentPath + "/" + name) != ERROR)
            return (system(("rm -rf " + currentPath + "/" + name).c_str()) == 0);
    }
    return false;
}

string CommandExecutor::ls(string currentPath) {
    DIR *dir = opendir(currentPath.c_str());
    struct dirent *entry;
    if (dir == NULL) return "";
    string files;
    while ((entry = readdir(dir)) != NULL) files += (string(entry->d_name) + "\n");
    return files;
}
