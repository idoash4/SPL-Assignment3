#include "StompProtocol.h"

StompProtocol::StompProtocol(Client& client) : client(&client), username(), receiptWaitingMessages(), subscriptions(), games(), receiptIdCounter(0), subscriptionIdCounter(0) {}

bool StompProtocol::process(const StompFrame& frame){
    if (frame.get_command() == "CONNECTED") {
        std::cout << "Login successful" << std::endl;
    } else if (frame.get_command() == "RECEIPT") {
        if (!receipt(frame)) {
            return false;
        }
    } else if (frame.get_command() == "MESSAGE") {
        receive_message(frame);
    } else if (frame.get_command() == "ERROR") {
        std::cout << frame.get_header("message") << std::endl;
        return false;
    }
    return true;
}

void StompProtocol::send_frame(StompFrame &frame, bool receipt) {
    if (receipt) {
        int receiptId = receiptIdCounter++;
        frame.set_header("receipt", std::to_string(receiptId));
        receiptWaitingMessages.emplace(receiptId, frame);
    }
    client->send_frame(frame);
}

void StompProtocol::connect(const std::string& username, const std::string& password) {
    this->username = username;
    StompFrame frame("CONNECT");
    frame.set_header("accept-version", "1.2");
    frame.set_header("host", "stomp.cs.bgu.ac.il");
    frame.set_header("login", username);
    frame.set_header("passcode", password);
    send_frame(frame);
}

void StompProtocol::disconnect() {
    StompFrame frame("DISCONNECT");
    send_frame(frame, true);
}

void StompProtocol::subscribe(const std::string& channel) {
    StompFrame frame("SUBSCRIBE");
    frame.set_header("destination", channel);
    frame.set_header("id", std::to_string(++subscriptionIdCounter));
    subscriptions.emplace(subscriptionIdCounter, channel);
    send_frame(frame, true);
}

void StompProtocol::unsubscribe(const std::string& channel) {
    StompFrame frame("UNSUBSCRIBE");
    frame.set_header("destination", channel);
    for (auto & subscription : subscriptions) {
        if (subscription.second == channel) {
            frame.set_header("id", std::to_string(subscription.first));
            break;
        }
    }
    send_frame(frame, true);
}

bool StompProtocol::receipt(const StompFrame &frame) {
    int receiptId = std::stoi(frame.get_header("receipt-id"));
    StompFrame request = receiptWaitingMessages.at(receiptId);
    receiptWaitingMessages.erase(receiptId);
    if (request.get_command() == "DISCONNECT") {
        return false;
    } else if (request.get_command() == "SUBSCRIBE") {
        subscribe_receipt(request);
    } else if (request.get_command() == "UNSUBSCRIBE") {
        unsubscribe_receipt(request);
    }
    return true;
}

void StompProtocol::subscribe_receipt(const StompFrame &frame) {
    std::string channel = frame.get_header("destination");
    int subscriptionId = std::stoi(frame.get_header("id"));
    subscriptions.emplace(subscriptionId, channel);
    std::cout << "Joined channel " << channel << std::endl;
}

void StompProtocol::unsubscribe_receipt(const StompFrame &frame) {
    std::string channel = frame.get_header("destination");
    int subscriptionId = std::stoi(frame.get_header("id"));
    subscriptions.erase(subscriptionId);
    std::cout << "Exited channel " << channel << std::endl;
}

void StompProtocol::report(const std::string& file_path) {
    names_and_events events = parseEventsFile(this->username, file_path);
    StompFrame frame("SEND");
    frame.set_header("destination", events.team_a_name + "_" + events.team_b_name);
    for (Event& event : events.events) {
        frame.set_body(event.to_string());
        send_frame(frame);
    }
}

void StompProtocol::receive_message(const StompFrame &frame) {
    std::string channel = frame.get_header("destination");
    Event event(frame.get_body());
    std::string game_name = event.get_team_a_name() + '_' + event.get_team_b_name();
    std::string user = event.get_user();

    // Add the game if it is new
    if (games.count(game_name) == 0) {
        games.emplace(game_name, std::map<std::string, Game>());
    }

    if (games.at(game_name).count(user)) {
        games.at(game_name).at(user).add_event(event);
    } else {
        games.at(game_name).emplace(user, Game(event.get_team_a_name(), event.get_team_b_name()));
        games.at(game_name).at(user).add_event(event);
    }
}

std::string StompProtocol::get_game_summary(std::string game_name, std::string user) {
    if (games.count(game_name) == 0 || games.at(game_name).count(user) == 0) {
        return "";
    }
    return games.at(game_name).at(user).to_string();
}


