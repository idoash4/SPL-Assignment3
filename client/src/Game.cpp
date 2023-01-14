#include "Game.h"

Game::Game(std::string team_a_name, std::string team_b_name) : team_a_name(std::move(team_a_name)), team_b_name(std::move(team_b_name)), general_stats(), team_a_stats(), team_b_stats(), events() {}

void Game::add_event(Event event) {
    for (auto &general_stat : event.get_game_updates()) {
        this->general_stats[general_stat.first] = general_stat.second;
    }
    for (auto &team_a_stat : event.get_team_a_updates()) {
        this->team_a_stats[team_a_stat.first] = team_a_stat.second;
    }
    for (auto &team_a_stat : event.get_team_b_updates()) {
        this->team_b_stats[team_a_stat.first] = team_a_stat.second;
    }
    this->events.push_back(event);
}

std::string Game::to_string() {
    std::string game_string;
    game_string += this->team_a_name + " vs " + this->team_b_name + "\n";
    game_string += "Game stats:\n";
    game_string += "General stats:\n";
    for (auto &general_stat : this->general_stats) {
        game_string += general_stat.first + ": " + general_stat.second + "\n";
    }
    game_string += this->team_a_name + " stats:\n";
    for (auto &team_a_stat : this->team_a_stats) {
        game_string += team_a_stat.first + ": " + team_a_stat.second + "\n";
    }
    game_string += this->team_b_name + " stats:\n";
    for (auto &team_b_stat : this->team_b_stats) {
        game_string += team_b_stat.first + ": " + team_b_stat.second + "\n";
    }
    game_string += "Game event reports:\n";
    for (auto &event : this->events) {
        game_string += std::to_string(event.get_time()) + " - " + event.get_name() + ":\n\n";
        game_string += event.get_discription() + "\n\n\n";
    }
    return game_string;
}
