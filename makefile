SRC_DIR=Source

COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

SRC_FILES_S := $(wildcard $(SRC_DIR)/Server/*.cpp, wildcard $(SRC_DIR)/Server/*.h, wildcard $(SRC_DIR)/Common/*.cpp, wildcard $(SRC_DIR)/Common/*.h)
SRC_FILES_C := $(wildcard $(SRC_DIR)/Client/*.cpp, wildcard $(SRC_DIR)/Client/*.h, wildcard $(SRC_DIR)/Common/*.cpp, wildcard $(SRC_DIR)/Common/*.h)

all: server.out client.out

server.out: $(SRC_FILES_S)
	${CC} ${SRC_DIR}/Server/*.cpp -o server.out

client.out: $(SRC_FILES_C)
	${CC} ${SRC_DIR}/Client/*.cpp -o client.out

clean:
	rm *.out
