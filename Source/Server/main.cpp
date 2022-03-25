#include <iostream>
#include <string>
#include "Server.h"
#include "CommandExecutor.h"

using namespace std;

int main() {
    Server::basePath = CommandExecutor::pwd();
    cout << Server::basePath << endl;
    string path = Server::basePath;
    string input, o;
    while (input != "quit") {
        cin >> input >> o;
        cout << CommandExecutor::rename(path, input, o) << endl;
    }

    return 0;
}
