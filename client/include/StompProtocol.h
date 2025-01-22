#pragma once

#include "../include/ConnectionHandler.h"
#include <queue>
#include "../include/Frame.h"

using namespace std;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    ConnectionHandler* connectionHandler;
    queue<Frame>* departingMessages;
    queue<Frame>* arrivingMessages;
public:
    Frame& createFrame(string line);
    // void messageClassifier(string line);
    // void connect(const string& username, string& passcode);
    // void send(string destination, string body);
    // void subscribe(string destination, string id);
    // void unsubscribe(string id);
    // void disconnect(string receipt);
};
