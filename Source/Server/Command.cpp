#include <vector>
#include "Command.h"
#include <sys/socket.h>
#include "CommandExecutor.h"

using namespace std;

bool Command::verify(string msg, bool count) {
    string word;
    vector<string> input;
    stringstream stream(msg);
    while (getline(stream, word, ' ')) input.push_back(word);

    if (!count) {
        if (input[0] == "user") return true;
        else if (input[0] == "pass") return true;
        else if (input[0] == "pwd") return true;
        else if (input[0] == "mkd") return true;
        else if (input[0] == "dele") return true;
        else if (input[0] == "ls") return true;
        else if (input[0] == "cwd") return true;
        else if (input[0] == "rename") return true;
        else if (input[0] == "retr") return true;
        else if (input[0] == "help") return true;
        else if (input[0] == "quit") return true;
    } else {
        if (input[0] == "user" && input.size() == 2) return true;
        else if (input[0] == "pass" && input.size() == 2) return true;
        else if (input[0] == "pwd" && input.size() == 1) return true;
        else if (input[0] == "mkd" && input.size() == 2) return true;
        else if (input[0] == "dele" && input.size() == 3) return true;
        else if (input[0] == "ls" && input.size() == 1) return true;
        else if ((input[0] == "cwd") && ((input.size() == 2) || (input.size() == 1))) return true;
        else if (input[0] == "rename" && input.size() == 3) return true;
        else if (input[0] == "retr" && input.size() == 2) return true;
        else if (input[0] == "help" && input.size() == 1) return true;
        else if (input[0] == "quit" && input.size() == 1) return true;
    }

    return false;
}

bool Command::verify(string msg, string cmd, int count) {
    string word;
    vector<string> input;
    stringstream stream(msg);
    while (getline(stream, word, ' ')) input.push_back(word);

    if (input.size() != count) return false;
    if (input[0] != cmd) return false;
    if (cmd == "retr") if (input[1].empty() || input[2].empty()) return false;

    return true;
}

bool Command::verify(string msg, string cmd, string branch, int count) {
    string word;
    vector<string> input;
    stringstream stream(msg);
    while (getline(stream, word, ' ')) input.push_back(word);

    if (input.size() != count) return false;
    if (input[0] != cmd) return false;
    if ((input[0] == cmd) && (input.size() == 1)) return true;
    if (cmd == "dele") if ((input[1] != branch) || (input[2].empty())) return false;
    if (cmd == "retr") if (input[1].empty() || input[2].empty()) return false;

    return true;
}

void Command::enterCredential(string msg, User *user) {
    string cmd, value;
    stringstream stream(msg);
    getline(stream, cmd, ' ');
    getline(stream, value, ' ');
    if (cmd == "user") user->username = value;
    if (cmd == "pass") user->password = value;
}

void Command::response(int fd, int code) {
    string msg;

    if (code == 214) msg += ("214\n" + CommandExecutor::help());
    else if (code == 331) msg = "331: User name okay, need password.";
    else if (code == 230) msg = "230: User logged in, proceed. Logged out if appropriate.";
    else if (code == 503) msg = "503: Bad sequence of commands.";
    else if (code == 430) msg = "430: Invalid username or password.";
    else if (code == 550) msg = "550: File unavailable.";
    else if (code == 221) msg = "221: Successful quit.";
    else if (code == 332) msg = "332: Need account for login.";
    else if (code == 501) msg = "501: Syntax error in parameters or arguments.";
    else if (code == 500) msg = "500: Error";
    else if (code == 425) msg = "425: Can't open data connection.";

    send(fd, msg.c_str(), msg.size(), 0);
}

void Command::response(int fd, int code, string branch) {
    string msg;

    if (code == 226) {
        msg += "226: ";
        if (branch == "ls") msg += "List transfer done.";
        else if (branch == "re") msg += "Successful Download.";
    }

    send(fd, msg.c_str(), msg.size(), 0);
}

void Command::response(int fd, int code, string branch, string name) {
    string msg;

    if (code == 250) {
        msg += "250: ";
        if ((branch == "-d") || (branch == "-f")) msg += (name + " deleted.");
        else if ((branch == "re") || (branch == "cwd")) msg += "Successful change.";
    } else if (code == 257) {
        msg += "257: ";
        if (branch == "pwd") msg += name;
        else if (branch == "mkd") msg += (name + " created.");
    }

    send(fd, msg.c_str(), msg.size(), 0);
}

string Command::getPath(string msg, int i) {
    string word;
    vector<string> input;
    stringstream stream(msg);
    while (getline(stream, word, ' ')) input.push_back(word);
    return input[i - 1];
}
