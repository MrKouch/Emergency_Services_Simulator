#include "../include/StompProtocol.h"
#include "../include/Frame.h"
#include "../include/ConnectionHandler.h"
#include "../include/event.h"
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
    isConnected(false), logOutReceiptID(-1), joinChannelReceiptID(-1), exitChannelReceiptID(-1), nextID(0),
    arrivingMessagesThread(),   
    connectionHandler(nullptr), usersIDs(), reportedEvents(), IDtoChannel(), channelToID() {
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
            generateSummary(args);
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
        SendFrame frame(event.get_channel_name(), event);
        reportedEvents.push_back(frame.toString());
    }
    // updating the summary will be done in the processMessage function - rememeber to join before requesting summary
    return reportedEvents;
}

// Helper function to convert epoch time to date time string
std::string epochToDateTime(int epoch) {
    std::time_t t = epoch;
    std::tm* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%d/%m/%y %H:%M");
    return oss.str();
}

// Helper function to generate summary of description
std::string generateSummaryHelper(const std::string& description) {
    if (description.length() <= 27) {
        return description;
    }
    return description.substr(0, 27) + "...";
}

void StompProtocol::generateSummary(const std::vector<std::string>& args) {
    std::string channel_name = args[1];
    std::string user = args[2];
    std::string file = args[3];

    // Retrieve events for the specified channel
    if (reportedEvents.find(channel_name) == reportedEvents.end()) {
        std::cerr << "Channel not found: " << channel_name << std::endl;
        return;
    }

    std::vector<Event> events = reportedEvents[channel_name];
    std::vector<Event> userEvents;

    // Filter events for the specified user
    for (const Event& event : events) {
        if (event.getEventOwnerUser() == user) {
            userEvents.push_back(event);
        }
    }

    // Generate stats
    int totalReports = userEvents.size();
    int activeCount = 0;
    int forcesArrivalCount = 0;

    for (const Event& event : userEvents) {
        if (event.get_general_information().count("active") && event.get_general_information().at("active") == "true") {
            activeCount++;
        }
        if (event.get_general_information().count("forces arrival at scene") && event.get_general_information().at("forces arrival at scene") == "true") {
            forcesArrivalCount++;
        }
    }

    // Sort events by date_time and then by event_name lexicographically
    std::sort(userEvents.begin(), userEvents.end(), [](const Event& a, const Event& b) {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time();
        }
        return a.get_name() < b.get_name();
    });

    // Generate report
    std::ofstream outFile(file);
    if (!outFile) {
        std::cerr << "Failed to open file: " << file << std::endl;
        return;
    }

    outFile << "Channel " << channel_name << "\n";
    outFile << "Stats:\n";
    outFile << "Total: " << totalReports << "\n";
    outFile << "active: " << activeCount << "\n";
    outFile << "forces arrival at scene: " << forcesArrivalCount << "\n";
    outFile << "Event Reports:\n";

    for (size_t i = 0; i < userEvents.size(); ++i) {
        const Event& event = userEvents[i];
        outFile << "Report_" << (i + 1) << ":\n";
        outFile << "city: " << event.get_city() << "\n";
        outFile << "date time: " << epochToDateTime(event.get_date_time()) << "\n";
        outFile << "event name: " << event.get_name() << "\n";
        outFile << "summary: " << generateSummaryHelper(event.get_description()) << "\n";
    }

    outFile.close();
    std::cout << "Summary generated and written to " << file << std::endl;
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
            cout << "[DEBUG] RECEIPT IS:" << endl;
            for(string arg : args) {
                cout << arg << endl;
            }
            disconnect();
            return "close the thread";
        }
        // Join a channel
        else if (receiptID == joinChannelReceiptID) {
            cout << "[DEBUG] Joining channel" << endl;
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

void StompProtocol:: processMessage(vector<string> args) {
    string sub = args[1];
    string msgID = args[2];
    string destination = args[3];
    Event event(args[4]);
    string channel = event.get_channel_name();
    if(reportedEvents.find(channel) == reportedEvents.end()) {
        reportedEvents[channel] = {};
    }
    reportedEvents[channel].push_back(event);
}



void StompProtocol :: disconnect() {
    if (connectionHandler != nullptr) {
        connectionHandler->close();
    }
    isConnected = false;
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




