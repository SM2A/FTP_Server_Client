#include <iostream>
#include <string>
#include "Server.h"
#include "CommandExecutor.h"

using namespace std;

int main() {
    Server::basePath = CommandExecutor::pwd();
    cout << Server::basePath << endl;
    string path = Server::basePath;
    string input;
    while (input != "quit") {
        cin >> input;
        cout << CommandExecutor::mkd(path, input) << endl;
    }

    return 0;
}
