#include "StompProtocol.h"
#include "Client.h"

Client::~Client() {
    delete stompProtocol;
    delete connectionHandler;
}

void Client::readFromKeyboard() {
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        std::stringstream stream(line);
        std::string command;
        stream >> command;

        if (command == "login") {
            login(stream);
        } else if (this->connectionHandler != nullptr) {
            if (command == "logout") {
                logout();
            } else if (command == "join") {
                joinGame(stream);
            } else if (command == "exit") {
                exitGame(stream);
            } else if (command == "report") {
                report(stream);
            } else if (command == "summary") {
                summary(stream);
            } else {
                std::cout << "Unknown command" << std::endl;
            }
        } else {
            std::cout << "You must login first" << std::endl;
        }
    }
}

void Client::readFromServer() {
    while (true) {
        // Wait for the client to connect
        std::unique_lock<std::mutex> lock(readMutex);
        readReady.wait(lock);
        while (true) {
            std::string message;
            connectionHandler->getFrameAscii(message, '\0');
            StompFrame frame = StompFrame::parseFrame(message);
            if (!stompProtocol->process(frame)) {
                disconnect();
                break;
            }
        }
    }
}

void Client::sendFrame(const StompFrame &frame) {
    connectionHandler->sendFrameAscii(frame.toString(), '\0');
}

void Client::login(std::stringstream &stream) {
    if(connectionHandler != nullptr) {
        std::cout << "The client is already logged in, log out before trying again" << std::endl;
        return;
    }

    std::string host, username, password;
    stream >> host;
    stream >> username;
    stream >> password;

    std::string ip = host.substr(0, host.find(':'));
    int port = std::stoi(host.substr(host.find(':') + 1));
    this->connectionHandler = new ConnectionHandler(ip, port);

    if (!this->connectionHandler->connect()) {
        std::cout << "Could not connect to server" << std::endl;
        this->connectionHandler = nullptr;
        return;
    }

    // Notify the reading thread
    std::lock_guard<std::mutex> lock(readMutex);
    readReady.notify_one();
    this->stompProtocol = new StompProtocol(*this);
    this->stompProtocol->connect(username, password);
}

void Client::disconnect() {
    connectionHandler->close();
    delete connectionHandler;
    delete stompProtocol;
    connectionHandler = nullptr;
    stompProtocol = new StompProtocol(*this);
    std::cout << "Disconnected" << std::endl;
}

void Client::logout() {
    this->stompProtocol->disconnect();
}

void Client::joinGame(std::stringstream &stream) {
    std::string gameName;
    stream >> gameName;
    this->stompProtocol->subscribe(gameName);
}

void Client::exitGame(std::stringstream &stream) {
    std::string gameName;
    stream >> gameName;
    this->stompProtocol->unsubscribe(gameName);
}

void Client::report(std::stringstream &stream) {

}

void Client::summary(std::stringstream &stream) {

}