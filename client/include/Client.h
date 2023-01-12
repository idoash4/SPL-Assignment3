#pragma once
#include <mutex>
#include <condition_variable>
#include "ConnectionHandler.h"
#include "StompFrame.h"

class StompProtocol;

class Client {
public:
    Client() = default;
    ~Client();
    // This class has a mutex and shouldn't be copied or moved
    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;
    Client(Client&& other) noexcept  = delete;
    Client& operator=(Client&& other)  = delete;
    void readFromKeyboard();

    void readFromServer();
    void sendFrame(const StompFrame& frame);
    void login(std::stringstream &stream);
    void logout();
    void disconnect();
    void joinGame(std::stringstream &stream);
    void exitGame(std::stringstream &stream);
    void report(std::stringstream &stream);
    void summary(std::stringstream &stream);
private:
    ConnectionHandler* connectionHandler;
    StompProtocol* stompProtocol;
    std::mutex readMutex;
    std::condition_variable readReady;
};