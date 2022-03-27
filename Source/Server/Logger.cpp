#include "Logger.h"
#include <fstream>

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
    file.open(FILE_NAME, ios_base::app);
    file << getDateTime() << " : "
         << username << " "
         << msg << endl;
    file.close();
}

void Logger::log(string username, string msg, string argument) {
    ofstream file;
    file.open(FILE_NAME, ios_base::app);
    file << getDateTime() << " : "
         << username << " "
         << msg << " "
         << argument << endl;
    file.close();
}
