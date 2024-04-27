#include <thread>
#include "Client.h"

int main(int argc, char *argv[]) {
    Client client;
    std::thread readThread(&Client::read_from_server, &client);
    client.read_from_keyboard();
    readThread.join();
	return 0;
}