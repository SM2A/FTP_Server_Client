#include <string>
#include <iostream>
#include "Server.h"
#include "Command.h"
#include <pthread.h>

using namespace std;

Server *server;

void *command(void *) {
    server->listenCommand();
    return nullptr;
}

void *data(void *) {
    server->listenData();
    return nullptr;
}

int main(int argc, char **argv) {

    server = Server::getInstance();

    try {
        if (argc == 2) server->init(string(argv[1]));
        else if (argc < 2) throw invalid_argument("No file entered");
        else if (argc > 2) throw invalid_argument("Fix arguments");

        server->startServer();

        pthread_t threads[2];
        pthread_create(&threads[0], nullptr, &command, nullptr);
        pthread_create(&threads[1], nullptr, &data, nullptr);
        for (unsigned long thread : threads) pthread_join(thread, nullptr);

    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
