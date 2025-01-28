#pragma once

#include <string>
#include <map>
#include <event.h>

using namespace std;

class Frame {
protected:
    string command_;
    map<string, string> headers_;
    string body_;

public:
    Frame(string command, map<string, string> headers, string body);
    string getCommand();
    map<string, string> getHeaders();
    string& getBody();
    string toString();
};

class ConnectFrame : public Frame {
public:
    ConnectFrame(const string& username, const string& passcode); // Use const references
};

class SendFrame : public Frame {
public:
    SendFrame(const string& destination, Event& event, const string& user); // Use const references
};

class SubscribeFrame : public Frame {
public:
    SubscribeFrame(const string& channelName, int subID, int receiptID); // Use const reference
};

class UnsubscribeFrame : public Frame {
public:
    UnsubscribeFrame(const string& subID, int receiptID); // Use const reference
};

class DisconnectFrame : public Frame {
public:
    DisconnectFrame(int receipt); // Use const reference
};