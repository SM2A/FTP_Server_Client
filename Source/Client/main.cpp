#include <iostream>
#include "Client.h"

using namespace std;

//Client* client;

int main(int argc, char **argv) {

    auto client = Client::getInstance();

    try {
        if (argc == 2) client->init(string(argv[1]));
        else if (argc < 2) throw invalid_argument("No file entered");
        else if (argc > 2) throw invalid_argument("Fix arguments");
        client->startCommand();
        while (true){
            client->sendCommand();
            client->receiveCommandResponse();
        }
    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
