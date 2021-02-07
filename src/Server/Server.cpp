#include "Server.h"
#include "../Network/NetworkCommon.h"
#include <SFML/Network/Packet.hpp>
#include <algorithm>
#include <iostream>

namespace {
    auto findPeer(std::vector<PendingClientSession>& pendingSessions, uint32_t incomingId)
    {
        return std::find_if(pendingSessions.begin(), pendingSessions.end(),
                            [&](const auto& peer) {
                                auto pendingId = peer.connection.handle->incomingPeerID;
                                return pendingId == incomingId;
                            });
    }
} // namespace

Server::Server()
    : m_host(4, 2)

{
}

void Server::onClientConnect(ENetPeer* peer)
{
    std::cout << "A client connected\n";

    PendingClientSession session;
    session.connection.handle = peer;
    m_pendingConnections.push_back(session);

    /*
        Player player;
        player.id = m_players.size() * 10;
        player.peer = peer;
        m_players.push_back(player);

        //auto p = makePacket(CommandToClient::PlayerId, m_salt);
        //p << player.id;

        ENetPacket* packet =
            enet_packet_create(p.getData(), p.getDataSize(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        */
}

void Server::onClientDisconnect(ENetPeer* peer)
{
    std::cout << "A client disconnected\n";
    std::cout << peer->incomingPeerID << std::endl;
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
                handlePacket(event.packet, event.peer);
                enet_packet_destroy(event.enetPacket);
                break;

        }
        // clang-format on
    }
}

void Server::handlePacket(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    using CTS = CommandToServer;

    // clang-format off
    switch (static_cast<CommandToServer>(packet.command)) {
        case CTS::PlayerClick:  /*      onPlayerClick(data, peer); */ break;
        case CTS::Handshake:            onHandshake(packet, peer);        break;
        case CTS::HandshakeResponse:    onHandshakeResponse(packet, peer);      break;
    }
    // clang-format on
}

void Server::onHandshake(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    std::cout << "Handshake received\n";

    auto itr = findPeer(m_pendingConnections, peer->incomingPeerID);
    if (itr != m_pendingConnections.end()) {
        itr->salt = packet.salt;
        itr->sendHandshakeChallenge(m_salt);
        std::cout << "Sending handshake challenge\n";
    }
}

void Server::onHandshakeResponse(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    auto itr = findPeer(m_pendingConnections, peer->incomingPeerID);
    if (itr != m_pendingConnections.end()) {
        PendingClientSession& pending = *itr;
        ENetPeer* pendingPeer = pending.connection.handle;

        if (pendingPeer->incomingPeerID == peer->incomingPeerID) {
            uint32_t salt = pending.salt ^ m_salt;
            if (salt == packet.salt) {
                pending.sendAcceptConnection(m_players.size());
            }
            else {
                pending.sendRejectConnection("Auth failed");
            }
        }

        m_pendingConnections.erase(itr);
    }
}

/*
void Server::onPlayerClick(sf::Packet& packet)
{
    uint16_t id = 0;
    float x = 0;
    float y = 0;

    packet >> id >> x >> y;
    std::cout << "Click at " << x << ", " << y << " from " << (int)id << std::endl;
}
*/
