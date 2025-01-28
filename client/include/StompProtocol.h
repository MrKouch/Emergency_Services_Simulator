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
    bool isConnected;
    int logOutReceiptID;
    int joinChannelReceiptID;
    int exitChannelReceiptID;
    int nextID;
    string loggedInUser;
    thread arrivingMessagesThread;
    std::shared_ptr<ConnectionHandler> connectionHandler; // Use shared_ptr
    unordered_map<string, vector<Event>> reportedEvents; // channel, reported events
    unordered_map<int, string> IDtoChannel; // ID, channel
    unordered_map<string, int> channelToID; // channel, ID

public:
    StompProtocol();
    ~StompProtocol();

    void createDepartingFrame(string& line);
    string processConnect(vector<string> args);
    vector<string> processSend(vector<string> args);
    string processSubscribe(vector<string> args);
    string processUnsubscribe(vector<string> args);
    string processDisconnect();

    void runArivingMessagesThread(std::shared_ptr<ConnectionHandler> connectionHandler);



    string processIncomingFrame(string& message);
    string processConnected();
    void processMessage(vector<string> args);
    string processReceipt(vector<string> args);


    bool joinChannel(string channel);
    
    

    void disconnect(); 


    
    vector<string> splitLine(const string& line);
    vector<string> splitFrameToLines(const string& frame);
    int generateNextID();


};
