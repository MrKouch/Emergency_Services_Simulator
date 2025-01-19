#include "../include/Frame.h"


Frame::Frame(std::string command, std::map<std::string, std::string> headers, std::string body) : command_(command), headers_(headers), body_(body) {}

std::string Frame::getCommand() {
    return command_;
}

std::map<std::string, std::string> Frame::getHeaders() {
    return headers_;
}

std::string Frame::getBody() {
    return body_;
}

class ConnectFrame : public Frame {
    public:
        ConnectFrame(string username, string passcode) : Frame("CONNECT",
            {{"accept - version:", "1.2"},
            {"host:", "stomp.cs.bgu.ac.il"},
            {"login:", username},
            {"passcode:", passcode}}, "") {}
};

class SendFrame : public Frame {
    public:
        SendFrame(string destination, string body) : Frame("SEND", {{"destination", destination}}, body) {}
};

class SubscribeFrame : public Frame {
    public:
        SubscribeFrame(string destination, string id) : Frame("SUBSCRIBE", {{"destination", destination}, {"id:", id}}, "") {}
};

class UnsubscribeFrame : public Frame {
    public:
        UnsubscribeFrame(string id) : Frame("UNSUBSCRIBE", {{"id:", id}}, "") {}
};

class DisconnectFrame : public Frame {
    public:
        DisconnectFrame(string receipt) : Frame("DISCONNECT", {{"receipt:", receipt}}, "") {}
};







