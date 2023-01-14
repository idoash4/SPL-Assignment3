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

std::string StompFrame::get_command() const {
    return command;
}

std::map<std::string, std::string> StompFrame::get_headers() const {
    return headers;
}

std::string StompFrame::get_header(const std::string& key) const {
    return headers.at(key);
}

void StompFrame::set_header(const std::string &key, std::string value) {
    headers[key] = std::move(value);
}

std::string StompFrame::get_body() const {
    return body;
}

void StompFrame::set_body(const std::string& body) {
    this->body = body;
}

std::string StompFrame::to_string() const {
    std::string output = command + "\n";
    for (auto &header : headers) {
        output += header.first + ":" + header.second + "\n";
    }
    output += "\n";

    if (!body.empty()) {
        output += body;
    }

    return output;
}

StompFrame StompFrame::parse_frame(const std::string& message) {
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

    // We can assume there is no null terminator in the string, so we can use it to read until the end of the stream
    std::getline(stream, body, '\0');

    return {command, headers, body};
}
