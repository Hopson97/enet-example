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
    : m_host(MAX_CONNECTIONS, 2)

{
}

void Server::broadcast(const sf::Packet& packet)
{
    for (auto& client : m_clients) {
        client.send(packet);
    }
}


void Server::onClientConnect(ENetPeer* peer)
{
    std::cout << "A client connected\n";
    PendingClientSession session;
    session.connection.handle = peer;
    m_pendingConnections.push_back(session);
}

void Server::onClientDisconnect(ENetPeer* peer)
{
    auto itr = m_clientsMap.find(peer->incomingPeerID);
    if (itr != m_clientsMap.end()) {

        auto index = itr->second;
        m_clients[index].disconnect();
        std::cout << "A client disconnected: " << m_clients[index].getPlayerId() << ".\n";
        // TODO m_world.removeEntity(m_clients[index].getPlayerId());
        // TODO broadcastPlayerLeave(m_clients[index].getPlayerId());
        m_clientsMap.erase(itr);
    }
    else {
        auto pending = findPeer(m_pendingConnections, peer->incomingPeerID);
        if (pending != m_pendingConnections.end()) {
            m_pendingConnections.erase(pending);
        }
    }
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

int Server::createClientSession(ENetPeer* peer, uint32_t salt)
{
    for (unsigned i = 0; i < m_clients.size(); i++) {
        if (!m_clients[i].isActive()) {
            std::cout << "Created client session for player ID " << i << ".\n\n";
            uint32_t playerId = i; // TODO = m_world.addEntity();


            sf::Packet broadcaster = makePacket(CommandToClient::PlayerJoined, m_salt);
            broadcaster << playerId;
            broadcast(broadcaster);

            m_clients[i].init(peer, salt, playerId);
            m_clientsMap[peer->incomingPeerID] = i;
            return i;
        }
    }
    return -1;
}

// clang-format off
void Server::handlePacket(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    using CTS = CommandToServer;

    switch (static_cast<CTS>(packet.command)) {
        case CTS::PlayerClick:          onPlayerClick(packet, peer);        break;
        case CTS::Handshake:            onHandshake(packet, peer);          break;
        case CTS::HandshakeResponse:    onHandshakeResponse(packet, peer);  break;
    }
}

#define AUTHENTICATE_PACKET                             \
    auto itr = m_clientsMap.find(peer->incomingPeerID); \
    if (itr == m_clientsMap.end()) {                    \
        return;                                         \
    }                                                   \
    auto& client = m_clients.at(itr->second);           \
    if (!client.verify(packet.salt)) {                  \
        return;                                         \
    }
// clang-format on

void Server::onHandshake(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    auto itr = findPeer(m_pendingConnections, peer->incomingPeerID);
    if (itr != m_pendingConnections.end()) {
        itr->salt = packet.salt;
        itr->sendHandshakeChallenge(m_salt);
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

                int slot = createClientSession(peer, salt);
                if (slot > -1) {
                    pending.sendAcceptConnection(m_clients[slot].getPlayerId());
                    // TODO Send the game data as well...
                }
                else {
                    pending.sendRejectConnection("Game is full");
                }
            }
            else {
                pending.sendRejectConnection("Auth failed");
            }
        }

        m_pendingConnections.erase(itr);
    }
}

void Server::onPlayerClick(NetworkEvent::Packet& packet, ENetPeer* peer)
{
    AUTHENTICATE_PACKET

    uint32_t id = 0;
    float x = 0;
    float y = 0;

    packet.data >> id >> x >> y;
    std::cout << "Click at " << x << ", " << y << " from " << (int)id << std::endl;
}
