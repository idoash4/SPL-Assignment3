#pragma once

#include "Client.h"
#include "Game.h"

class Client;

class StompProtocol
{
public:
    StompProtocol(Client& client);
    StompProtocol(const StompProtocol& other) = delete;
    StompProtocol& operator=(const StompProtocol& other) = delete;
    StompProtocol(StompProtocol&& other) noexcept  = delete;
    StompProtocol& operator=(StompProtocol&& other)  = delete;
    bool process(const StompFrame& frame);
    void connect(const std::string& username, const std::string& password);
    void disconnect();
    void subscribe(const std::string& channel);
    void unsubscribe(const std::string& channel);
    void report(const std::string& file_path);
    std::string get_game_summary(std::string game_name, std::string user);
private:
    Client* client;
    std::string username;
    std::map<int, StompFrame> receiptWaitingMessages;
    std::map<int, std::string> subscriptions;
    std::map<std::string, std::map<std::string, Game>> games;
    int receiptIdCounter;
    int subscriptionIdCounter;
    void send_frame(StompFrame& frame, bool receipt = false);
    bool receipt(const StompFrame& frame);
    void subscribe_receipt(const StompFrame &frame);
    void unsubscribe_receipt(const StompFrame &frame);
    void receive_message(const StompFrame &frame);
};
