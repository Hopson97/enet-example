#include "Server.h"
#include <SFML/Network/Packet.hpp>
#include <iostream>
#include "NetworkCommon.h"


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
    std::cout << peer->incomingPeerID << std::endl;
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

void Server::tick()
{
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

void Server::onPlayerClick(sf::Packet& packet)
{
    uint16_t id = 0;
    float x = 0;
    float y = 0;

    packet >> id >> x >> y;
    std::cout << "Click at " << x << ", " << y << " from " << (int)id << std::endl;
}