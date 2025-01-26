#pragma once

#include <string>
#include <map>

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
    SendFrame(const string& destination, const string& body); // Use const references
};

class SubscribeFrame : public Frame {
public:
    SubscribeFrame(const string& channelName, int id); // Use const reference
};

class UnsubscribeFrame : public Frame {
public:
    UnsubscribeFrame(const string& id); // Use const reference
};

class DisconnectFrame : public Frame {
public:
    DisconnectFrame(int receipt); // Use const reference
};