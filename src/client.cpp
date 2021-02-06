#include "Keyboard.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <array>
#include <enet/enet.h>
#include <iostream>

int main()
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    //
    // Create client
    //
    ENetHost* clientHost;
    clientHost = enet_host_create(nullptr, 1, 1, 0, 0);

    if (!clientHost) {
        std::cerr << "Could not create client rip.\n";
    }
    std::cout << "Starting client\n";

    //
    // Connect client to the server
    //
    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, "192.168.0.20");
    address.port = 54321;

    peer = enet_host_connect(clientHost, &address, 1, 0);
    if (!peer) {
        std::cerr << "Could not connect...\n";
        return 1;
    }

    if (enet_host_service(clientHost, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection succeeded.\n";
    }
    else {
        enet_peer_reset(peer);
        puts("Connection to some.server.net:1234 failed.");
        return 1;
    }

    sf::RenderWindow window({1280, 720}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    Keyboard keyboard;

    sf::Clock dt;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            keyboard.update(e);
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                default:
                    break;
            }
        }

        ENetEvent event;
        while (enet_host_service(clientHost, &event, 100) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    printf("A new client connected");
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
                    break;
            }
        }

        window.clear();
        window.display();
    }

    enet_deinitialize();
}