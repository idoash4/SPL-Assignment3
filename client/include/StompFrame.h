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

    static StompFrame parseFrame(const std::string& message);
    std::string getCommand() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getHeader(const std::string& key) const;
    void setHeader(const std::string &key, std::string value);
    std::string getBody() const;
    std::string toString() const;
};