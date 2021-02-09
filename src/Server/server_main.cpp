#include "../World/World.h"
#include "Server.h"
#include <atomic>
#include <iostream>
#include <thread>

int main()
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    std::atomic_bool isRunning{true};

    std::thread console([&]() {
        std::string line;
        while (isRunning) {
            std::cout << "Type a command.\n> ";
            std::getline(std::cin, line);
            if (line == "exit") {
                isRunning = false;
            }
        }
    });

    std::cout << "Starting server.\n";
    World world;
    Server server(world);
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        server.tick();
    }

    console.join();
    enet_deinitialize();
}