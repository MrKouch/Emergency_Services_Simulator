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
    ConnectionHandler* connectionHandler;
    unordered_map<string, unordered_map<string, int>> users; // username, IDs per channel
    unordered_map<string, int> channelsIDs; // channelName, ID
    unordered_map<string, vector<Frame>> summaries; // username, reported events
    bool isConnected;
    string loggedInUser;
    
public:
    StompProtocol();
    void createDepartingFrame(string line);
    string processConnect(vector<string> args);
    string processSend(vector<string> args);
    string processSubscribe(vector<string> args);
    string processUnsubscribe(vector<string> args);
    string processDisconnect(vector<string> args);



    void processIncomingFrame(string message);
    string processConnected(vector<string> args);
    string processMessage(vector<string> args);
    string processReceipt(vector<string> args);
    string processError(vector<string> args);
    string addtoSummary(string user, Frame event);  


    
    vector<string> splitLine(const string& line);
    vector<string> splitMessagetoLines(const string& message);


    int generateID(string channelName);
};
