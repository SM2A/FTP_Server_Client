#include <fstream>
#include "Logger.h"
#include <iostream>

using namespace std;

string Logger::getDateTime() {
    char buff[80];
    time_t now = time(nullptr);
    struct tm currentTime = *localtime(&now);
    strftime(buff, sizeof(buff), "%Y-%m-%d %X", &currentTime);
    return string(buff);
}

void Logger::log(string username, string msg) {
    ofstream file;
    string log = getDateTime() + " : "
                 + username + " "
                 + msg;
    file.open(FILE_NAME, ios_base::app);
    file << log << endl;
    file.close();
    cout << log << endl;
}

void Logger::log(string username, string msg, string argument) {
    ofstream file;
    string log = getDateTime() + " : "
                 + username + " "
                 + msg + " "
                 + argument;
    file.open(FILE_NAME, ios_base::app);
    file << log << endl;
    file.close();
    cout << log << endl;
}
