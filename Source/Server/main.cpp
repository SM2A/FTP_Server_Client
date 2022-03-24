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
        try {
            path = CommandExecutor::cwd(path, input);
        } catch (exception e) {
            cerr << e.what() << endl;
        }
        cout << path << endl;
    }

    return 0;
}
