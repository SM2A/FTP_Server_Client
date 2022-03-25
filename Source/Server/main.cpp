#include <iostream>
#include <string>
#include "Server.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        if (argc == 2) Server::getInstance()->init(string(argv[1]));
        else if (argc < 2) cerr << "No file entered" << endl;
        else if (argc > 2) cerr << "Fix arguments" << endl;
    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
