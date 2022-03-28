#include "Data.h"
#include <sys/socket.h>

using namespace std;

void Data::response(int fd, string data) {
    send(fd, data.c_str(), data.size(), 0);
}
