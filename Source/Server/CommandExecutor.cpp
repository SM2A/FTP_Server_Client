#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <dirent.h>
#include <algorithm>
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
        if ((currentPath.size() == 1) && (currentPath == "/")) return currentPath;
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

bool CommandExecutor::rename(string currentPath, string currentName, string afterName) {
    return (std::rename((currentPath + "/" + currentName).c_str(), (currentPath + "/" + afterName).c_str()) == 0);
}

string CommandExecutor::help() {
    string helper;
    helper += "user <username>, Username of user for authentication\n";
    helper += "pass <password>, Password of user for authentication\n";
    helper += "pwd, Path of current working directory\n";
    helper += "cwd <path>, Chane working directory to path\n";
    helper += "dele <path>, Delete file or directory\n";
    helper += "\t -f <file name>, Delete file with file name\n";
    helper += "\t -d <path>, Delete directory with path\n";
    helper += "ls, List of files in current working directory\n";
    helper += "rename <from> <to>, Rename file\n";
    helper += "retr <name>, Fetch file with given name\n";
    helper += "help, Print user manual\n";
    helper += "quit, Exit FTP server";
    return helper;
}

long CommandExecutor::getFileSize(string path) {
    ifstream file(path, std::ifstream::ate | std::ifstream::binary);
    if (!file.good()) return -1;
    return file.tellg();
}

string CommandExecutor::getFileContent(string path) {
    ifstream file(path);
    string content, line;
    while (getline(file, line)) content += (line + "\n");
    file.close();
    return content;
}

string CommandExecutor::getFileName(std::string path) {
    string fileName;
    for (int i = path.size() - 1; i >= 0; i--) {
        if (path[i] == '/') break;
        fileName += path[i];
    }
    reverse(fileName.begin(), fileName.end());
    return fileName;
}
