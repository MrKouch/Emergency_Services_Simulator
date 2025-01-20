#include "../include/StompProtocol.h"
#include "../include/Frame.h"

#include <iostream>
#include <sstream>

using namespace std;

StompProtocol::StompProtocol() {
    connectionHandler = new ConnectionHandler("localhost", 7777);
    departingMessages = new std::queue<Frame>();
    arrivingMessages = new std::queue<Frame>();
}

StompProtocol::~StompProtocol() {
    delete connectionHandler;
    delete departingMessages;
    delete arrivingMessages;
}

Frame& StompProtocol::createFrame(string line) {
    istringstream iss(line);
    string command;
    iss >> command;
    if (command == "CONNECT") {
        string username;
        string passcode;
        iss >> username >> passcode;
        ConnectFrame* frame = new ConnectFrame(username, passcode);
        return *frame;
    }
    else if (command == "SEND") {
        string destination;
        string body;
        iss >> destination;
        getline(iss, body);
        SendFrame* frame = new SendFrame(destination, body);
        return *frame;
    }
    else if (command == "SUBSCRIBE") {
        string destination;
        string id;
        iss >> destination >> id;
        SubscribeFrame* frame = new SubscribeFrame(destination, id);
        return *frame;
    }
    else if (command == "UNSUBSCRIBE") {
        string id;
        iss >> id;
        UnsubscribeFrame* frame = new UnsubscribeFrame(id);
        return *frame;
    }
    else if (command == "DISCONNECT") {
        string receipt;
        iss >> receipt;
        DisconnectFrame* frame = new DisconnectFrame(receipt);
        return *frame;
    }
    else {
        throw "Invalid command";
    }
}

