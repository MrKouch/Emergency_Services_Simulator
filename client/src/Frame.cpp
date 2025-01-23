#include "../include/Frame.h"
#include "../include/StompProtocol.h"


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

std::string Frame::toString() {
    std::string frame = command_ + "\n";
    for (auto const& header : headers_) {
        frame += header.first + ":" + header.second + "\n";
    }
    frame += "\n" + body_ + "\0";
    return frame;
}

class ConnectFrame : public Frame {
    public:
        ConnectFrame(string username, string passcode) : Frame("CONNECT",
            {{"accept - version:", "1.2"},
            {"host:", "stomp.cs.bgu.ac.il"},
            {"login:", username},
            {"passcode:", passcode}}, "") {}
};

class SubscribeFrame : public Frame {
    public:
        SubscribeFrame(string destination, string channelName) : Frame("SUBSCRIBE", {{"destination", destination}, {"id:", StompProtocol :: protocol}}, "") {}
};

class SendFrame : public Frame {
    public:
        SendFrame(string destination, string body) : Frame("SEND", {{"destination", destination}}, body) {}
};


class UnsubscribeFrame : public Frame {
    public:
        UnsubscribeFrame(string id) : Frame("UNSUBSCRIBE", {{"id:", id}}, "") {}
};

class DisconnectFrame : public Frame {
    public:
        DisconnectFrame(string receipt) : Frame("DISCONNECT", {{"receipt:", receipt}}, "") {}
};

class ConnectedFrame : public Frame {
    public:
        ConnectedFrame(string version) : Frame("CONNECTED", {{"version:", version}}, "") {}
};

class MessageFrame : public Frame {
    public:
        MessageFrame(string destination, string body) : Frame("MESSAGE", {{"destination", destination}}, body) {}
};

class ReceiptFrame : public Frame {
    public:
        ReceiptFrame(string receipt) : Frame("RECEIPT", {{"receipt-id:", receipt}}, "") {}
};

class ErrorFrame : public Frame {
    public:
        ErrorFrame(string message) : Frame("ERROR", {{"message:", message}}, "") {}
};









