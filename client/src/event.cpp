#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <regex>
using json = nlohmann::json;

Event::Event(std::string user, std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : user(user), team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

const std::string &Event::get_user() const {
    return this->user;
}

Event::Event(const std::string &frame_body) : user(""), team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
    std::stringstream ss(frame_body);
    std::string line;
    while (std::getline(ss, line)) {
        std::string key = line.substr(0, line.find(':'));
        std::string value = line.substr(line.find(':') + 1);
        // Remove leading space if exists
        value = std::regex_replace(value, std::regex("^ "), "");

        if (key == "user") {
            this->user = value;
        } else if (key == "team a") {
            this->team_a_name = value;
        } else if (key == "team b") {
            this->team_b_name = value;
        } else if (key == "event name") {
            this->name = value;
        } else if (key == "time") {
            this->time = std::stoi(value);
        } else if (key.find("updates") != std::string::npos) {
            std::map<std::string, std::string>* map = nullptr;
            if (key == "general game updates"){
                map = &this->game_updates;
            } else if (key == "team a updates"){
                map = &this->team_a_updates;
            } else if (key == "team b updates"){
                map = &this->team_b_updates;
            }
            if (map != nullptr) {
                while (ss.peek() == '\t') {
                    std::getline(ss, line);
                    std::string key2 = line.substr(1, line.find(":") - 1);
                    std::string value2 = line.substr(line.find(":") + 1);
                    // Remove leading space if exists
                    value2 = std::regex_replace(value2, std::regex("^ "), "");

                    (*map)[key2] = value2;
                }
                continue;
            }
        } else if (key == "description") {
            // We can assume there is no null terminator in the string, so we can use it to read until the end of the stream
            std::getline(ss, this->description, '\0');
        }
    }
}

std::string Event::to_string() const {
    std::string event;
    event += "user: " + this->user + "\n";
    event += "team a: " + this->team_a_name + "\n";
    event += "team b: " + this->team_b_name + "\n";
    event += "event name: " + this->name + "\n";
    event += "time: " + std::to_string(this->time) + "\n";
    event += "general game updates:\n";
    for (auto const &x : this->game_updates) {
        event += "\t" + x.first + ": " + x.second + "\n";
    }
    event += "team a updates:\n";
    for (auto const &x : this->team_a_updates) {
        event += "\t" + x.first + ": " + x.second + "\n";
    }
    event += "team b updates:\n";
    for (auto const &x : this->team_b_updates) {
        event += "\t" + x.first + ": " + x.second + "\n";
    }
    event += "description:\n" + this->description;
    return event;
}

names_and_events parseEventsFile(std::string user, std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(user, team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}