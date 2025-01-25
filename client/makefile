CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompEMIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompEMIClient: bin/ConnectionHandler.o bin/StrompProtocol.o bin/StompClient.o bin/event.o bin/Frame.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/StrompProtocol.o bin/StompClient.o bin/event.o bin/Frame.o $(LDFLAGS)
	./bin/StompEMIClient 127.0.0.1 7777

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/StrompProtocol.o: src/StrompProtocol.cpp
	g++ $(CFLAGS) -o bin/StrompProtocol.o src/StrompProtocol.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Frame.o: src/Frame.cpp
	g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

.PHONY: clean
clean:
	rm -f bin/*
