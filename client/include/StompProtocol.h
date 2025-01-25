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
    unordered_map<string, unordered_map<string, int>> existingUsers; // username, IDs per channel
    unordered_map<string, int> channelsIDs; // channelName, ID
    unordered_map<string, vector<Frame>> summaries; // username, reported events
    int receiptID;
    thread arrivingMessagesThread;
    bool isConnected;
    pair<string, int> loggedInUser; // username, receiptID
    
public:
    StompProtocol();
    ~StompProtocol();

    string& getLoggedInUser();
    int getReceiptID();

    void createDepartingFrame(string& line);
    string processConnect(vector<string> args);
    string processSend(vector<string> args);
    string processSubscribe(vector<string> args);
    string processUnsubscribe(vector<string> args);
    string processDisconnect();

    void runArivingMessagesThread(std::shared_ptr<ConnectionHandler> connectionHandler);



    string processIncomingFrame(string& message);
    string processConnected();
    string processMessage(vector<string> args);
    string processReceipt(vector<string> args);
    string processError(vector<string> args);
    void openSummary(string& user);
    string addtoSummary(string user, Frame event);  


    
    vector<string> splitLine(const string& line);
    vector<string> splitFrameToLines(const string& frame);
    void assignAndIncrementReceiptID();


};
