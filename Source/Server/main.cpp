#include <iostream>
#include <string>
#include "Server.h"
#include "CommandExecutor.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        cout<<CommandExecutor::pwd()<<endl;
        if (argc == 2) Server::getInstance()->init(string(argv[1]));
        else if (argc < 2) throw invalid_argument("No file entered");
        else if (argc > 2) throw invalid_argument("Fix arguments");
        Server::getInstance()->startServer();
        Server::getInstance()->listenCommand();
    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
