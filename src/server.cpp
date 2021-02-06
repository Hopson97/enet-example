#include <SFML/Network/Packet.hpp>
#include <atomic>
#include <enet/enet.h>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <cstdint>

struct Player {
    
    float x = 0;
    float y = 0;
};

int main()
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    ENetAddress address;
    ENetHost* serverHost;

    address.host = ENET_HOST_ANY;
    address.port = 54321;

    serverHost = enet_host_create(&address, 32, 1, 0, 0);
    if (!serverHost) {
        std::cerr << "Could not create server rip.\n";
    }
    std::cout << "Starting server \n";

    std::vector<ENetPeer*> peers;

    while (true) {

        ENetPacket* packet = enet_packet_create("yee", 4, ENET_PACKET_FLAG_RELIABLE);

        for (auto p : peers) {
            enet_peer_send(p, 0, packet);
        }

        ENetEvent event;
        while (enet_host_service(serverHost, &event, 100) > 0) {
            std::cout << "heck " << event.type << std::endl;
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "A client connected\n";
                    peers.push_back(event.peer);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("disconnected.\n");
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    printf("A packet of length %u containing %s was received on "
                           "channel %u.\n",
                           (unsigned)event.packet->dataLength, event.packet->data,
                           event.channelID);
                    enet_packet_destroy(event.packet);
                    break;

                default:
                    std::cout << "Unknown event. " << event.type << std::endl;
                    ;
                    break;
            }
        }
    }

    enet_deinitialize();
}