#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include <unordered_map>

#include <iostream>
#include <sstream>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler* connectionHandler) {
    departingMessages = new queue<string>();
    loggedInUser = "";
    users = {};
    summaries = {};
}

void StompProtocol::createDepartingFrame(string line) {
    string frame;
    vector<string> args = split(line);
    string command = args[0];
    if(loggedInUser == "" && command != "login") {
        cout << "please login first" << endl;
        return;
    }
    if (command == "login") {
        frame = processConnect(args);
    }
    else if (command == "SEND") {
        frame = processSend(args);
    }
    else if (command == "SUBSCRIBE") {
        frame = processSubscribe(args);
    }
    else if (command == "UNSUBSCRIBE") {
        frame = processUnsubscribe(args);
    }
    else if (command == "DISCONNECT") {
        frame = processDisconnect(args);
    }
    else {
        throw "Invalid command";
    }
        departingMessages->push(frame);
}

string StompProtocol:: processConnect(vector<string> args) {
    string username = args[2];
    string passcode = args[3];
    if(loggedInUser == "") {
        //start from here
    }
    ConnectFrame frame(username, passcode);
    summaries.insert({username, {}});
    // remember to reset the user to "" if an error is 
    return frame.toString();
}

string StompProtocol:: processSend(vector<string> args) {
    string destination = args[1];
    string body = args[2];
    SendFrame frame(destination, body);
    // updating the summary will be done in the processMessage function - rememeber to join before requesting summary
    return frame.toString();
}

string StompProtocol:: processSubscribe(vector<string> args) {
    string destination = args[1];
    string id = args[2];
    SubscribeFrame frame(destination, id);
    users.insert({loggedInUser, id});
    return frame.toString();
}

string StompProtocol:: processUnsubscribe(vector<string> args) {
    string id = args[1];
    UnsubscribeFrame frame(id);
    return frame.toString();
}

string StompProtocol:: processDisconnect(vector<string> args) {
    string receipt = args[1];
    DisconnectFrame frame(receipt);
    return frame.toString();
}


void StompProtocol:: processIncomingFrame(string message) {
    vector<string> args = split(message);
    string command = args[0];
    string frame;
    if (command == "CONNECTED") {
        cout << "Login successful" << endl;

        frame = processConnected(args);
    }
    else if (command == "MESSAGE") {
        frame = processMessage(args);
    }
    else if (command == "RECEIPT") {
        frame = processReceipt(args);
    }
    else if (command == "ERROR") {
        frame = processError(args);
    }
    else {
        throw "Invalid command";
    }
    cout << frame << endl;
}

string StompProtocol:: processConnected(vector<string> args) {
    string version = args[1];
    ConnectedFrame frame(version);
    return frame.toString();
}

string StompProtocol:: processMessage(vector<string> args) {
    string destination = args[1];
    string body = args[2];
    string user = args[3];
    MessageFrame frame(destination, body);
    addtoSummary(user, frame);
    return frame.toString();
}

string StompProtocol:: processReceipt(vector<string> args) {
    string receipt = args[1];
    ReceiptFrame frame(receipt);
    return frame.toString();
}

string StompProtocol:: processError(vector<string> args) {
    string message = args[1];
    ErrorFrame frame(message);
    return frame.toString();
}



vector<string> StompProtocol :: split(const string& line) {
    istringstream iss(line);
    vector<string> words;
    string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}


