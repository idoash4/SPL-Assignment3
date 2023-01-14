#pragma once
#include "string"
#include "map"

class StompFrame{
private:
    std::string command;
    std::map<std::string, std::string> headers;
    std::string body;
public:
    // Constructors
    StompFrame(std::string command, std::map<std::string, std::string> headers, std::string body);
    StompFrame(std::string command, std::map<std::string, std::string> headers);
    explicit StompFrame(std::string command);

    static StompFrame parse_frame(const std::string& message);
    std::string get_command() const;
    std::map<std::string, std::string> get_headers() const;
    std::string get_header(const std::string& key) const;
    void set_header(const std::string &key, std::string value);
    std::string get_body() const;
    void set_body(const std::string& body);
    std::string to_string() const;
};