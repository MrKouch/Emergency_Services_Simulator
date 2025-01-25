#include "../include/Frame.h"
#include "../include/StompProtocol.h"

using namespace std;

Frame::Frame(string command, map<string, string> headers, string body) : command_(command), headers_(headers), body_(body) {}

string& Frame::getCommand() {
    return command_;
}

map<string, string> Frame::getHeaders() {
    return headers_;
}

string& Frame::getBody() {
    return body_;
}

string Frame::toString() {
    string frame = command_ + "\n";
    for (auto const& header : headers_) {
        frame += header.first + ":" + header.second + "\n";
    }
    frame += "\n" + body_;
    return frame;
}


ConnectFrame:: ConnectFrame(string username, string passcode) : Frame("CONNECT",
            {{"accept-version", "1.2"},
            {"host", "stomp.cs.bgu.ac.il"},
            {"login", username},
            {"passcode", passcode}}, "") {}



SubscribeFrame:: SubscribeFrame(string channelName) : Frame("SUBSCRIBE", {{"destination", channelName}/*, {"id:", use the global variable}*/}, "") {}



SendFrame:: SendFrame(string destination, string body) : Frame("SEND", {{"destination", destination}}, body) {}




UnsubscribeFrame:: UnsubscribeFrame(string id) : Frame("UNSUBSCRIBE", {{"id:", id}}, "") {}



DisconnectFrame:: DisconnectFrame(int receipt) : Frame("DISCONNECT", {{"receipt:", to_string(receipt)}}, "") {}



// ConnectedFrame:: ConnectedFrame(string version) : Frame("CONNECTED", {{"version:", version}}, "") {}

// MessageFrame:: MessageFrame(string destination, string body) : Frame("MESSAGE", {{"destination", destination}}, body) {}

// ReceiptFrame:: ReceiptFrame(string receipt) : Frame("RECEIPT", {{"receipt-id:", receipt}}, "") {}

// ErrorFrame:: ErrorFrame(string message) : Frame("ERROR", {{"message:", message}}, "") {}










