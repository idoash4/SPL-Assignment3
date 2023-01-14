CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: clean StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/StompFrame.o bin/Client.o bin/StompProtocol.o bin/Game.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/StompFrame.o bin/Client.o bin/StompProtocol.o bin/Game.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp

bin/Client.o: src/Client.cpp
	g++ $(CFLAGS) -o bin/Client.o src/Client.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/Game.o: src/Game.cpp
	g++ $(CFLAGS) -o bin/Game.o src/Game.cpp

.PHONY: clean
clean:
	rm -f bin/*
	