#include "NetworkCommon.h"

#include "NetworkHost.h"
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
    Server();

    void run();

  private:
    void onClientConnect(ENetPeer* peer);
    void onClientDisconnect(ENetPeer* peer);

    void handlePacket(NetworkEvent::Packet& packet);
    void onPlayerClick(sf::Packet& packet);

  private:
    std::vector<Player> m_players;
    // ENetHost* m_host;

    NetworkHost m_host;
};

Server::Server()
    : m_host(4, 2)
{
}

void Server::onClientConnect(ENetPeer* peer)
{
    std::cout << "A client connected\n";
    Player player;
    player.id = m_players.size() * 10;
    player.peer = peer;
    m_players.push_back(player);

    auto p = makePacket(CommandToClient::PlayerId);
    p << player.id;

    ENetPacket* packet =
        enet_packet_create(p.getData(), p.getDataSize(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::onClientDisconnect(ENetPeer* peer)
{
    std::cout << "A client disconnected\n";
}

void Server::handlePacket(NetworkEvent::Packet& packet)
{
    sf::Packet& data = packet.data;

    // clang-format off
    switch (static_cast<CommandToServer>(packet.command)) {
        case CommandToServer::PlayerClick:  onPlayerClick(data);  break;
    }
    // clang-format on
}

void Server::run()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        NetworkEvent event;
        while (m_host.pollEvent(event)) {
            // clang-format off
            switch(event.type) {
                case NetworkEventType::Connection:      onClientConnect(event.peer);    break;
                case NetworkEventType::Disconnection:   onClientDisconnect(event.peer); break;
                case NetworkEventType::Timeout:         onClientDisconnect(event.peer); break;
                case NetworkEventType::Data:
                    handlePacket(event.packet);
                    enet_packet_destroy(event.enetPacket);
                    break;

            }
            // clang-format on
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

    Server server;
    server.run();

    enet_deinitialize();
}