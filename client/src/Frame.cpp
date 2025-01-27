#include "Frame.h"
#include <string>
#include <map>

using namespace std;

Frame::Frame(string command, map<string, string> headers, string body)
    : command_(command), headers_(headers), body_(body) {}

string Frame::getCommand() {
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

ConnectFrame::ConnectFrame(const string& username, const string& passcode)
    : Frame("CONNECT",
            {{"accept-version", "1.2"},
             {"host", "stomp.cs.bgu.ac.il"},
             {"login", username},
             {"passcode", passcode}}, "") {}

SubscribeFrame::SubscribeFrame(const string& channelName, int subID, int receiptID)
    : Frame("SUBSCRIBE", {{"destination", channelName}, {"id", to_string(subID)}, {"receipt", to_string(receiptID)}}, "") {}



SendFrame::SendFrame(const string& destination, Event& event)
    : Frame("SEND", {{"destination", destination}}, "") {
        body_ = "event_name:" + event.get_name() + "\n" +
                "city:" + event.get_city() + "\n" +
                "date_time:" + to_string(event.get_date_time()) + "\n" +
                "description:" + event.get_description() + "\n" +
                "general_information:" + "\n" +
                "active:" + event.get_general_information().at("active") + "\n" +
                "forces_arrival_at_scene:" + event.get_general_information().at("forces_arrival_at_scene") + "\n";
    }

UnsubscribeFrame::UnsubscribeFrame(const string& subID, int receiptID)
    : Frame("UNSUBSCRIBE", {{"id", subID}, {"receipt", to_string(receiptID)}}, "") {}

DisconnectFrame::DisconnectFrame(int receipt)
    : Frame("DISCONNECT", {{"receipt", to_string(receipt)}}, "") {}



// ConnectedFrame:: ConnectedFrame(string version) : Frame("CONNECTED", {{"version:", version}}, "") {}

// MessageFrame:: MessageFrame(string destination, string body) : Frame("MESSAGE", {{"destination", destination}}, body) {}

// ReceiptFrame:: ReceiptFrame(string receipt) : Frame("RECEIPT", {{"receipt-id:", receipt}}, "") {}

// ErrorFrame:: ErrorFrame(string message) : Frame("ERROR", {{"message:", message}}, "") {}










