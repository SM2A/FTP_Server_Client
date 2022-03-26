#include <iostream>
#include <string>
//#include <sstream>
#include "Server.h"
#include "CommandExecutor.h"
#include "Command.h"
#include <pthread.h>

using namespace std;

void *command(void *) {
    Server::getInstance()->listenCommand();
    return nullptr;
}

void *data(void *) {
    Server::getInstance()->listenData();
    return nullptr;
}

int main(int argc, char **argv) {
    try {
        if (argc == 2) Server::getInstance()->init(string(argv[1]));
        else if (argc < 2) throw invalid_argument("No file entered");
        else if (argc > 2) throw invalid_argument("Fix arguments");
        Server::getInstance()->startServer();


        pthread_t threads[2];
        pthread_create(&threads[0], nullptr, &command, nullptr);
        pthread_create(&threads[1], nullptr, &data, nullptr);
        for (unsigned long thread : threads) pthread_join(thread, nullptr);

//        Server::getInstance()->listenCommand();
//        Server::getInstance()->listenData();

    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
