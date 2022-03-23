SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_S := $(wildcard $(SRC_DIR)/Server/*.cpp, wildcard $(SRC_DIR)/Server/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Client/*.cpp, wildcard $(SRC_DIR)/Client/*.h)

all: server.out client.out

server.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Server/main.cpp -o server.out

client.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Client/main.cpp -o client.out

clean:
	rm *.out
