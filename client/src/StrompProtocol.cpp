#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/ConnectionHandler.h"
#include <vector>
#include <thread>

#include <unordered_map>

#include <iostream>
#include <sstream>

using namespace std;

extern string HOST;
extern short PORT;

StompProtocol::StompProtocol() : isConnected(false), nextID(0), logOutID(-1) ,connectionHandler(nullptr), usersIDs(), reportedEvents(), IDtoChannel(), channelToID() {}
StompProtocol::~StompProtocol() {
    if(connectionHandler != nullptr) {
        connectionHandler->close();
    }
    if(arrivingMessagesThread.joinable()) {
        arrivingMessagesThread.join();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
// Client Frames Handling

void StompProtocol::createDepartingFrame(string& line) {
    string frame;
    vector<string> args = splitLine(line);
    string command = args[0];
    if(!isConnected) {
        if(command != "login") {
            cout << "please first login" << endl;
            return;
        }
        else {
            if(args.size() != 5 or args[1] != HOST or args[2] != to_string(PORT)) {
                cout << "invalid login command!" << endl;
                return;
            }
            frame = processConnect(args);
        }
    }
    else {
        if(command == "login") {
            cout << "The client is already logged in, log out before trying gain" << endl;
            return;
        }
        else if (command == "logout") {
            frame = processDisconnect();
        }
        else if (command == "join") {
            frame = processSubscribe(args);
        }
        // else if (command == "SEND") {
        //     frame = processSend(args);
        // }
        // else if (command == "UNSUBSCRIBE") {
        //     frame = processUnsubscribe(args);
        // }
        else {
            cout << "Illegal command, please try a different one" << endl;
            return;
        }
        if(connectionHandler == nullptr) {
            throw runtime_error("Connection handler is null - logical problem - debug!");
            return;
        }
    }
        connectionHandler->sendMessage(frame);
}

string StompProtocol:: processConnect(vector<string> args) {
    if(arrivingMessagesThread.joinable()) {
        arrivingMessagesThread.join(); // wait for the thread to finish logging out the previous user
    }
    connectionHandler = std::make_shared<ConnectionHandler>(args[1], std::stoi(args[2]));
    if(connectionHandler->connect()) {
        arrivingMessagesThread = thread(&StompProtocol::runArivingMessagesThread, this, connectionHandler);
        // arrivingMessagesThread starts automatically when created
        string username = args[3];
        string passcode = args[4];
        ConnectFrame frame(username, passcode);
        isConnected = true;
        return frame.toString();
    }
    else {
        throw runtime_error("Connection failed");
    }
    
}

string StompProtocol:: processDisconnect() {
    logOutID = generateNextID();
    DisconnectFrame frame(logOutID);
    return frame.toString();
}

string StompProtocol:: processSubscribe(vector<string> args) {
    string channelName = args[1];
    if(channelToID.find(channelName) == channelToID.end()) {
        int subID = generateNextID();
        channelToID[channelName] = subID;
        IDtoChannel[subID] = channelName;
    }
    SubscribeFrame frame(channelName, channelToID[channelName]);
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




void StompProtocol:: runArivingMessagesThread(std::shared_ptr<ConnectionHandler> connectionHandler) {
    while(true) {
		string answer;
        if (!connectionHandler->getMessage(answer)) {
			std::cout << "Disconnected. Exiting...\n" << std::endl;
			break;
		}
        answer.resize(answer.length() - 1);
        string incomingMessage = processIncomingFrame(answer);
        if(incomingMessage == "close the thread") {
            break;
        }
        else {
            cout << incomingMessage << endl;
        }
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Server Frames Handling

string StompProtocol:: processIncomingFrame(string& frame) {
    string output;
    vector<string> args = splitFrameToLines(frame);
    string command = args[0];
    if (command == "CONNECTED") {
        isConnected = true;
        output = processConnected();
    }
    // else if (command == "MESSAGE") {
    //     output = processMessage(args);
    // }
    else if (command == "RECEIPT") {
        output = processReceipt(args);
    }
    // else if (command == "ERROR") {
    //     output = processError(args);
    // }
    else {
        throw runtime_error("got an invalid frame from server!");
    }
    return output;
}

string StompProtocol:: processConnected() {
        isConnected = true;
        return "Login successful";
}

// string StompProtocol:: processMessage(vector<string> args) {
//     string destination = args[1];
//     string body = args[2];
//     string user = args[3];
//     MessageFrame frame(destination, body);
//     addtoSummary(user, frame);
//     return frame.toString();
// }

string StompProtocol:: processReceipt(vector<string> args) {
    vector<string> header = splitLine(args[1]);
    string receipt = header[1];
    try {
        if (receipt.empty() || !std::all_of(receipt.begin(), receipt.end(), ::isdigit)) {
            throw std::invalid_argument("Invalid receipt ID");
        }
        int receiptID = std::stoi(receipt);
        if (receiptID == logOutID) {
            disconnect();
            std::cout << "Logged out" << std::endl;
            return "close the thread";
        }
        // Join a channel
        else if (IDtoChannel.find(receiptID) != IDtoChannel.end()) {
            string channelToJoin = IDtoChannel.find(receiptID)->second;
            joinChannel(channelToJoin);
            return "Joined channel " + channelToJoin;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid receipt ID: " << e.what() << std::endl;
        return "Invalid receipt ID";
    }
    return receipt;
}


//if the channel is not in the reportedEvents map, add it with an empty vector
void StompProtocol:: joinChannel(string channel) {
    if(reportedEvents.find(channel) == reportedEvents.end()) {
        reportedEvents[channel] = {};
    }
}

void StompProtocol :: disconnect() {
    if (connectionHandler != nullptr) {
        connectionHandler->close();
    }
    isConnected = false;
    logOutID = -1;
}

// string StompProtocol:: processError(vector<string> args) {
//     string message = args[1];
//     ErrorFrame frame(message);
//     return frame.toString();
// }



vector<string> StompProtocol :: splitLine(const string& line) {
    vector<string> args = {};
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
    args.push_back(word);
    return args;
}

vector<string> StompProtocol :: splitFrameToLines(const string& frame) {
    vector<string> args;
    string line = "";
    for(char c : frame) {
        if(c == '\0') {
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


int StompProtocol:: generateNextID() {
    int output = nextID;
    nextID++;
    return output;
}




