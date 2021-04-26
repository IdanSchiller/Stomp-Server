CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -pthread

all: client
	g++ -o bin/client bin/main.o bin/connectionHandler.o bin/KeyBoardThread.o $(LDFLAGS)

client: bin/main.o bin/connectionHandler.o bin/KeyBoardThread.o

bin/main.o: src/main.cpp
	g++ $(CFLAGS) -o bin/main.o src/main.cpp

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/KeyBoardThread.o: src/KeyBoardThread.cpp
	g++ $(CFLAGS) -o bin/KeyBoardThread.o src/KeyBoardThread.cpp

.PHONY: clean
clean:
	rm -f bin/*
