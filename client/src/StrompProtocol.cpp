#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include <unordered_map>

#include <iostream>
#include <sstream>

using namespace std;

StompProtocol::StompProtocol() {
    users = {};
    summaries = {};
    channelsIDs = {};
    isConnected = false;
    loggedInUser = "";
    connectionHandler = nullptr;
}

void StompProtocol::createDepartingFrame(string line) {
    string frame;
    vector<string> args = splitLine(line);
    string command = args[0];
    if(!isConnected) {
        if(command != "login") {
            cout << "please first login" << endl;
            return;
        }
        else {
            frame = processConnect(args);
        }
    }
    else {
        if(command == "login") {
            cout << "The client is already logged in, log out before trying again" << endl;
            return;
        }
        else if (command == "join") {
            frame = processSubscribe(args);
        }
        else if (command == "SEND") {
            frame = processSend(args);
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
        if(connectionHandler == nullptr) {
           throw "Connection handler is null - logical problem - debug!";
            return;
        }
        connectionHandler->sendLine(frame);
    }
}

string StompProtocol:: processConnect(vector<string> args) {
    connectionHandler = new ConnectionHandler(args[1], stoi(args[2]));
    string username = args[3];
    string passcode = args[4];
    ConnectFrame frame(username, passcode);
    isConnected = true;
    loggedInUser = username;
    return frame.toString();
}

string StompProtocol:: processSubscribe(vector<string> args) {
    string destination = args[1];
    string channelName = args[2];
    if(channelsIDs.find(channelName) == channelsIDs.end()) {
        
    }
    else {
        int id = generateID(channelName);
        channelsIDs[channelName] = id;
        
    }
        
    SubscribeFrame frame(destination, channelName);
    return frame.toString();
}

string StompProtocol:: processSend(vector<string> args) {
    string destination = args[1];
    string body = args[2];
    SendFrame frame(destination, body);
    // updating the summary will be done in the processMessage function - rememeber to join before requesting summary
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

int StompProtocol:: generateID(string channelName) {
    
}

/////////////////////////////////////////////////////////////////////////////////////////
// Server Frames Handling

void StompProtocol:: processIncomingFrame(string message) {
    vector<string> args = splitMessagetoLines(message);
    string command = args[0];
    string frame;
    if (command == "CONNECTED") {
        cout << "Login successful" << endl;
        isConnected = true;
        //frame = processConnected(args);
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

// string StompProtocol:: processConnected(vector<string> args) {
//     string version = args[1];
//     ConnectedFrame frame(version);
//     return frame.toString();
// }

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



vector<string> StompProtocol :: splitLine(const string& line) {
    vector<string> args;
    string word = "";
    for(char c : line) {
        if(c == ' ' or c == ':') {
            args.push_back(word);
            word = "";
        }
        else {
            word += c;
        }
    }
    return args;
}

vector<string> StompProtocol :: splitMessagetoLines(const string& message) {
    vector<string> args;
    string line = "";
    for(char c : message) {
        if(c= '\0') {
            return args;
        }
        if(c == '\n') {
            args.push_back(line);
            line = "";
        }
        else {
            line += c;
        }
    }
    return args;
}





