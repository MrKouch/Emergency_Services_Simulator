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
    queue<string>* departingMessages = new queue<string>();
    unordered_map<string, string> users;    // username, ID
    unordered_map<string, vector<Frame>> summaries; // username, reported events
    string loggedInUser;
public:
    StompProtocol(ConnectionHandler* connectionHandler);
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


    
    vector<string> split(const string& text);
};
