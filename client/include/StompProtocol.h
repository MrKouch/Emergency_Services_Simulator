#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/Frame.h"
#include <queue>
#include <unordered_map>


using namespace std;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    std::shared_ptr<ConnectionHandler> connectionHandler; // Use shared_ptr
    unordered_map<string, int> usersIDs; // user, ID
    unordered_map<string, vector<Event>> reportedEvents; // channel, reported events
    unordered_map<int, string> IDtoChannel; // ID, channel
    unordered_map<string, int> channelToID; // channel, ID
    int nextID;
    thread arrivingMessagesThread;
    bool isConnected;
    int logOutReceiptID;
    int joinChannelReceiptID;
    int exitChannelReceiptID;

public:
    StompProtocol();
    ~StompProtocol();

    void createDepartingFrame(string& line);
    string processConnect(vector<string> args);
    vector<string> processSend(vector<string> args);
    string processSubscribe(vector<string> args);
    string processUnsubscribe(vector<string> args);
    void generateSummary(const vector<string>& args);
    string generateSummary(const string& description);
    string processDisconnect();

    void runArivingMessagesThread(std::shared_ptr<ConnectionHandler> connectionHandler);



    string processIncomingFrame(string& message);
    string processConnected();
    string processMessage(vector<string> args);
    string processReceipt(vector<string> args);

    bool joinChannel(string channel);
    
    

    void disconnect(); 


    
    vector<string> splitLine(const string& line);
    vector<string> splitFrameToLines(const string& frame);
    int generateNextID();


};
