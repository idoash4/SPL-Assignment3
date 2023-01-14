#pragma once
#include <string>
#include "event.h"

class Game {
private:
    std::string team_a_name;
    std::string team_b_name;
    std::map<std::string , std::string> general_stats;
    std::map<std::string , std::string> team_a_stats;
    std::map<std::string , std::string> team_b_stats;
    std::vector<Event> events;
public:
    Game(std::string team_a_name, std::string team_b_name);
    void add_event(Event event);
    std::string to_string();
};