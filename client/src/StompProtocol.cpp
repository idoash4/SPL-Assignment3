#include "StompProtocol.h"

StompProtocol::StompProtocol(Client& client) {
    this->client = &client;
    receiptWaitingMessages = std::map<int, StompFrame>();
    receiptIdCounter = 0;
}

bool StompProtocol::process(const StompFrame& frame){
    if (frame.getCommand() == "CONNECTED") {
        std::cout << "Login successful" << std::endl;
    } else if (frame.getCommand() == "RECEIPT") {
        if (!receipt(frame)) {
            return false;
        }
    } else if (frame.getCommand() == "ERROR") {
        std::cout << frame.getHeader("message") << std::endl;
        return false;
    }
    return true;
}

void StompProtocol::sendFrame(StompFrame &frame, bool receipt) {
    if (receipt) {
        int receiptId = receiptIdCounter++;
        frame.setHeader("receipt", std::to_string(receiptId));
        receiptWaitingMessages.emplace(receiptId, frame);
    }
    client->sendFrame(frame);
}

void StompProtocol::connect(const std::string& username, const std::string& password) {
    StompFrame frame("CONNECT");
    frame.setHeader("accept-version", "1.2");
    frame.setHeader("host", "stomp.cs.bgu.ac.il");
    frame.setHeader("login", username);
    frame.setHeader("passcode", password);
    sendFrame(frame);
}

void StompProtocol::disconnect() {
    StompFrame frame("DISCONNECT");
    sendFrame(frame);
}

void StompProtocol::subscribe(const std::string& channel) {
    StompFrame frame("SUBSCRIBE");
    frame.setHeader("destination", channel);
    frame.setHeader("id", std::to_string(++subscriptionIdCounter));
    subscriptions.emplace(subscriptionIdCounter, channel);
    sendFrame(frame, true);
}

void StompProtocol::unsubscribe(const std::string& channel) {
    StompFrame frame("UNSUBSCRIBE");
    frame.setHeader("destination", channel);
    for (auto & subscription : subscriptions) {
        if (subscription.second == channel) {
            frame.setHeader("id", std::to_string(subscription.first));
            break;
        }
    }
    sendFrame(frame, true);
}

bool StompProtocol::receipt(const StompFrame &frame) {
    int receiptId = std::stoi(frame.getHeader("receipt-id"));
    StompFrame request = receiptWaitingMessages.at(receiptId);
    receiptWaitingMessages.erase(receiptId);
    if (request.getCommand() == "DISCONNECT") {
        return false;
    } else if (request.getCommand() == "SUBSCRIBE") {
        subscribeReceipt(request);
    } else if (request.getCommand() == "UNSUBSCRIBE") {
        unsubscribeReceipt(request);
    }
    return true;
}

void StompProtocol::subscribeReceipt(const StompFrame &frame) {
    std::string channel = frame.getHeader("destination");
    int subscriptionId = std::stoi(frame.getHeader("id"));
    subscriptions.emplace(subscriptionId, channel);
    std::cout << "Joined channel " << channel << std::endl;
}

void StompProtocol::unsubscribeReceipt(const StompFrame &frame) {
    std::string channel = frame.getHeader("destination");
    int subscriptionId = std::stoi(frame.getHeader("id"));
    subscriptions.erase(subscriptionId);
    std::cout << "Exited channel " << channel << std::endl;
}



