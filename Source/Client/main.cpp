#include <iostream>
#include "Client.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        if (argc == 2) Client::getInstance()->init(string(argv[1]));
        else if (argc < 2) throw invalid_argument("No file entered");
        else if (argc > 2) throw invalid_argument("Fix arguments");
        Client::getInstance()->startCommand();
    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
