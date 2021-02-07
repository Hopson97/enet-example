#include "NetCommon.h"
#include <SFML/Network/Packet.hpp>
#include <atomic>
#include <cstdint>
#include <enet/enet.h>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

struct Player {
    uint16_t id;
    float x = 0;
    float y = 0;
    ENetPeer* peer = nullptr;
};

class Server {
  public:
    Server(ENetHost* serverHost);

    void run();

  private:
    void onClientConnect(ENetPeer* peer);
    void onClientDisconnect(ENetPeer* peer);

    void handlePacket(ENetPacket* packet);

    void onPlayerClick(sf::Packet& packet);

  private:
    std::vector<Player> m_players;
    ENetHost* m_host;
};

Server::Server(ENetHost* serverHost)
    : m_host(serverHost)
{
}

void Server::onClientConnect(ENetPeer* peer)
{
    std::cout << "A client connected\n";
    Player player;
    player.id = m_players.size() * 10;
    player.peer = peer;
    m_players.push_back(player);

    sf::Packet p;
    p << CommandToClient::PlayerId << player.id;

    ENetPacket* packet =
        enet_packet_create(p.getData(), p.getDataSize(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::onClientDisconnect(ENetPeer* peer)
{
    std::cout << "A client disconnected\n" << peer->eventData << std::endl;
}

void Server::handlePacket(ENetPacket* packet)
{
    printf("A packet of length %u containing %s was received\n",
           (unsigned)packet->dataLength, packet->data);
    sf::Packet p;
    p.append(packet->data, packet->dataLength + 1);

    CommandToServer cmd;
    p >> cmd;

    // clang-format off
    switch (cmd) {
        case CommandToServer::PlayerClick:  onPlayerClick(p);  break;
    }
    // clang-format on
}

void Server::run()
{
    while (true) {

        //  ENetPacket* packet = enet_packet_create("yee", 4, ENET_PACKET_FLAG_RELIABLE);

        //    for (auto& p : peers) {
        //        enet_peer_send(p.peer, 0, packet);
        //    }

        ENetEvent event;
        while (enet_host_service(m_host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    onClientConnect(event.peer);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    onClientDisconnect(event.peer);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                    onClientDisconnect(event.peer);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    handlePacket(event.packet);

                    enet_packet_destroy(event.packet);
                    break;

                default:
                    std::cout << "Unknown event. " << event.type << std::endl;
                    ;
                    break;
            }
        }
    }
}

void Server::onPlayerClick(sf::Packet& packet)
{
    uint16_t id = 0;
    float x = 0;
    float y = 0;

    packet >> id >> x >> y;
    std::cout << "Click at " << x << ", " << y << std::endl;
}

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
        return -1;
    }
    std::cout << "Starting server \n";

    Server server(serverHost);
    server.run();

    enet_deinitialize();
}