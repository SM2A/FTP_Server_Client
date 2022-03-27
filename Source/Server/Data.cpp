#include "Data.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>

using namespace std;

void Data::response(int fd, string data) {
    send(fd, data.c_str(), data.size(), 0);
}
