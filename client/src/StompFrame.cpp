#include "StompFrame.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>

StompFrame::StompFrame(std::string command) {
    this->command = std::move(command);
}

StompFrame::StompFrame(std::string command, std::map<std::string, std::string> headers) {
    this->command = std::move(command);
    this->headers = std::move(headers);
}

StompFrame::StompFrame(std::string command, std::map<std::string, std::string> headers, std::string body) {
    this->command = std::move(command);
    this->headers = std::move(headers);
    this->body = std::move(body);
}

std::string StompFrame::getCommand() const {
    return command;
}

std::map<std::string, std::string> StompFrame::getHeaders() const {
    return headers;
}

std::string StompFrame::getHeader(const std::string &key) const {
    return headers.at(key);
}

void StompFrame::setHeader(const std::string &key, std::string value) {
    headers[key] = std::move(value);
}

std::string StompFrame::getBody() const {
    return body;
}

std::string StompFrame::toString() const {
    std::string output = command + "\n";
    for (auto &header : headers) {
        output += header.first + ":" + header.second + "\n";
    }
    output += "\n";

    if (!body.empty()) {
        output += body + "\n";
    }

    return output;
}

StompFrame StompFrame::parseFrame(const std::string& message) {
    std::stringstream stream(message);
    std::string command, line, body;
    std::getline(stream, command, '\n');

    std::map<std::string, std::string> headers;
    while (std::getline(stream, line)) {
        if (line.empty()) {
            break;
        }
        std::stringstream headerStream(line);
        std::string key, value;
        std::getline(headerStream, key, ':');
        std::getline(headerStream, value);
        headers[key] = value;
    }

    std::getline(stream, body);

    return {command, headers, body};
}
