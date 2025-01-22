#pragma once

#include <string>
#include <iostream>
#include <map>

using namespace std;

class Frame {
    private:
        string command_;
        map<string, string> headers_;
        string body_;
    
    public:
        Frame(string command, map<string, string> headers, string body);
        string getCommand();
        map<string, string> getHeaders();
        string getBody();
        string toString();
};

class ConnectFrame : public Frame {
    public:
        ConnectFrame(string username, string passcode);
};

class SendFrame : public Frame {
    public:
        SendFrame(string destination, string body);
};


class SubscribeFrame : public Frame {
    public:
        SubscribeFrame(string destination, string id);
};

class UnsubscribeFrame : public Frame {
    public:
        UnsubscribeFrame(string id);
};

class DisconnectFrame : public Frame {
    public:
        DisconnectFrame(string receipt);
};

