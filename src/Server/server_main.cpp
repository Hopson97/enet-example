#include "Server.h"
#include <thread>

int main()
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    Server server;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        server.tick();
    }

    enet_deinitialize();
}