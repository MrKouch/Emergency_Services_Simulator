#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "SummaryGenerator.h"
#include <vector>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <sstream>

using namespace std;

extern string HOST;
extern short PORT;

StompProtocol::StompProtocol() : 
    isConnected(false), logOutReceiptID(-1), joinChannelReceiptID(-1), exitChannelReceiptID(-1), nextID(0), loggedInUser(""),
    arrivingMessagesThread(),   
    connectionHandler(nullptr), reportedEvents(), IDtoChannel(), channelToID() {
}
StompProtocol::~StompProtocol() {
    if(connectionHandler != nullptr) {
        connectionHandler->close();
    }
    if(arrivingMessagesThread.joinable()) {
        arrivingMessagesThread.join();
    }
    cout << "[DEBUG] StompProtocol destroyed" << endl;
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
            if(frame == "Already subscribed to this channel") {
                return;
            }
            cout << "joined channel " << args[1] << endl;
        }
        else if (command == "exit") {
            frame = processUnsubscribe(args);
            if (frame == "you are not subscribed to channel" + args[1]) {
                cout << "you are not subscribed to channel " << args[1] << endl;
                return;
            }
            cout << "exited channel " << args[1] << endl;
        }
        else if (command == "report") {
            if (args.size() != 2) {
                cout << "report command needs 1 args: {file}" << endl;
                return;
            }
            else {
                vector<string> sendMe = processSend(args);
                for(string frame : sendMe) {
                    connectionHandler->sendMessage(frame);
            }
                cout << "reported" << endl;
            }
        }
        else if (command == "summary") {
            SummaryGenerator::generateSummary(args, reportedEvents);
        }
        else {
            cout << "Illegal command, please try a different one" << endl;
            return;
        }
        if(connectionHandler == nullptr) {
            throw runtime_error("Connection handler is null - logical problem - debug!");
            return;
        }
    }
        if(command != "report") {
            connectionHandler->sendMessage(frame);
        }
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
        loggedInUser = username;
        return frame.toString();
    }
    else {
        throw runtime_error("Connection failed");
    }
    
}

string StompProtocol:: processDisconnect() {
    logOutReceiptID = generateNextID();
    DisconnectFrame frame(logOutReceiptID);
    return frame.toString();
}

string StompProtocol:: processSubscribe(vector<string> args) {
    string channel = args[1];
    if(joinChannel(channel)) {
        joinChannelReceiptID = generateNextID();
        SubscribeFrame frame(channel, channelToID[channel], joinChannelReceiptID);
        return frame.toString();
    }
    else {
        return "Already subscribed to this channel";
    }
}

//if the channel is not in the reportedEvents map, add it with an empty vector
bool StompProtocol:: joinChannel(string channel) {
    if(reportedEvents.find(channel) == reportedEvents.end()) {
        reportedEvents[channel] = {};
    }
    if(channelToID.find(channel) == channelToID.end()) {
        int subID = generateNextID();
        channelToID[channel] = subID;
        IDtoChannel[subID] = channel;
        return true;
    }
    return false;
}

string StompProtocol:: processUnsubscribe(vector<string> args) {
    string channel = args[1];
    if(channelToID.find(channel) == channelToID.end()) {
        return "you are not subscribed to channel" + channel;
    }
    else {
        string subID = to_string(channelToID[channel]);
        int eraseMEID = channelToID[channel];
        channelToID.erase(channel);
        IDtoChannel.erase(eraseMEID);
        exitChannelReceiptID = generateNextID();
        UnsubscribeFrame frame(subID, exitChannelReceiptID);
        return frame.toString();
    }
}

vector<string> StompProtocol:: processSend(vector<string> args) {
    // string destination = args[1];
    string file = args[1];
    names_and_events events = parseEventsFile(file);
    vector<string> reportedEvents;
    for(Event event : events.events) {
        SendFrame frame(event.get_channel_name(), event, loggedInUser);
        reportedEvents.push_back(frame.toString());
        cout << "[DEBUG] send frame is: " << endl;
        cout << frame.toString() << endl;
    }
    // updating the summary will be done in the processMessage function - rememeber to join before requesting summary
    return reportedEvents;
}





void StompProtocol:: runArivingMessagesThread(std::shared_ptr<ConnectionHandler> connectionHandler) {
    while(true) {
		string answer;
        if (!connectionHandler->getMessage(answer)) {
			std::cout << "Disconnected. Exiting..." << std::endl;
			break;
		}
        answer.resize(answer.length() - 1);
        string incomingMessage = processIncomingFrame(answer);
        if(incomingMessage == "close the thread") {
            std::cout << "Logged out" << std::endl;
            break;
        }
        else if (incomingMessage == "default") {
            continue;
        }
        else {
            cout << incomingMessage << endl;
        }
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// Server Frames Handling

string StompProtocol:: processIncomingFrame(string& frame) {
    string output = "default";
    vector<string> args = splitFrameToLines(frame);
    // cout << "[DEBUG] incoming frame is: " << frame << endl;
    for(string arg : args) {
        cout << arg << endl;
    }
    string command = args[0];
    if (command == "CONNECTED") {
        isConnected = true;
        output = processConnected();
    }
    else if (command == "RECEIPT") {
        output = processReceipt(args);
    }
    else if (command == "MESSAGE") {
        processMessage(args);
    }
    else if (command == "ERROR") {
        disconnect();
        output = frame;
    }
    else {
        cout << "[DEBUG] incoming frame is: " << frame << endl;
        throw runtime_error("got an invalid frame from server!");
    }
    return output;
}

string StompProtocol:: processConnected() {
        isConnected = true;
        return "Login successful";
}


string StompProtocol:: processReceipt(vector<string> args) {
    vector<string> header = splitLine(args[1]);
    string receipt = header[1];
    try {
        if (receipt.empty() || !std::all_of(receipt.begin(), receipt.end(), ::isdigit)) {
            throw std::invalid_argument("Invalid receipt ID");
        }
        int receiptID = std::stoi(receipt);
        if (receiptID == logOutReceiptID) {
            for(string arg : args) {
                cout << arg << endl;
            }
            disconnect();
            return "close the thread";
        }
        // Join a channel
        else if (receiptID == joinChannelReceiptID) {
            cout << "[DEBUG] Joining channel. RECEIPT IS:" << endl;
            for (string arg : args) {
                cout << arg << endl;
            }
        }
        // Exit a channel
        else if (receiptID == exitChannelReceiptID) {
            cout << "[DEBUG] Exiting channel" << endl;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid receipt ID: " << e.what() << std::endl;
        return "Invalid receipt ID";
    }
    return receipt;
}

void StompProtocol::processMessage(vector<string> args) {
    for (string arg : args) {
        cout << arg << endl;
    }
    std::unordered_map<std::string, std::string> argMap;
    for (const std::string& arg : args) {
        size_t pos = arg.find(':');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            argMap[key] = value;
        }
    }

    // Check if all required keys exist in the map
    if (argMap.find("destination") == argMap.end() ||
        argMap.find("event_name") == argMap.end() ||
        argMap.find("city") == argMap.end() ||
        argMap.find("date_time") == argMap.end() ||
        argMap.find("description") == argMap.end() ||
        argMap.find("general_information") == argMap.end() ||
        argMap.find("active") == argMap.end() ||
        argMap.find("forces_arrival_at_scene") == argMap.end()) {
        std::cerr << "Error: Missing required fields in the message" << std::endl;
        return;
    }

    std::string destination = argMap["destination"];
    std::string event_name = argMap["event_name"];
    std::string city = argMap["city"];
    int date_time = std::stoi(argMap["date_time"]);
    std::string description = argMap["description"];
    std::string general_information = argMap["general_information"];
    std::string active = argMap["active"];
    std::string forces_arrival_at_scene = argMap["forces_arrival_at_scene"];

    std::cout << "[DEBUG]: general_information: " << general_information << std::endl;

    std::map<std::string, std::string> general_info_map;
    general_info_map["general_information"] = general_information;
    general_info_map["active"] = active;
    general_info_map["forces_arrival_at_scene"] = forces_arrival_at_scene;

    Event event(destination, city, event_name, date_time, description, general_info_map);
    std::string channel = event.get_channel_name();
    if (reportedEvents.find(channel) == reportedEvents.end()) {
        reportedEvents[channel] = {};
    }
    reportedEvents[channel].push_back(event);
}



void StompProtocol :: disconnect() {
    if (connectionHandler != nullptr) {
        connectionHandler->close();
    }
    isConnected = false;
    loggedInUser = "";
    logOutReceiptID = -1;
}

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
    for (string arg : args) {
        cout << arg << endl;
    }
    return args;
}

vector<string> StompProtocol::splitFrameToLines(const string& frame) {
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
    // Handle the case where the frame does not end with a newline
    if (!line.empty()) {
        args.push_back(line);
    }
    return args;
}


int StompProtocol:: generateNextID() {
    int output = nextID;
    nextID++;
    return output;
}





