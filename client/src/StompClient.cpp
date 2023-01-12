#include <thread>
#include "Client.h"

int main(int argc, char *argv[]) {
    Client client;
    std::thread readThread(&Client::readFromServer, &client);
    client.readFromKeyboard();
    readThread.join();
	return 0;
}