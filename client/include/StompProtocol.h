#pragma once

#include "Client.h"

class Client;

class StompProtocol
{
private:
    Client* client;
    std::map<int, StompFrame> receiptWaitingMessages;
    std::map<int, std::string> subscriptions;
    int receiptIdCounter;
    int subscriptionIdCounter;
    void sendFrame(StompFrame& frame, bool receipt = false);
    bool receipt(const StompFrame& frame);
    void subscribeReceipt(const StompFrame &frame);
    void unsubscribeReceipt(const StompFrame &frame);
public:
    StompProtocol(Client& client);
    bool process(const StompFrame& frame);
    void connect(const std::string& username, const std::string& password);
    void disconnect();
    void subscribe(const std::string& channel);
    void unsubscribe(const std::string& channel);
};
