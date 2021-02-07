#pragma once

#include "../Network/NetworkHost.h"
#include "ClientSession.h"
#include <array>
#include <cstdint>
#include <enet/enet.h>
#include <unordered_map>
#include <vector>

constexpr static unsigned MAX_CONNECTIONS = 4;

class Server {
  public:
    Server();

    void tick();

  private:
    int createClientSession(ENetPeer* peer, uint32_t salt);

    void onClientConnect(ENetPeer* peer);
    void onClientDisconnect(ENetPeer* peer);
    void handlePacket(NetworkEvent::Packet& packet, ENetPeer* peer);

    void onHandshake(NetworkEvent::Packet& packet, ENetPeer* peer);
    void onHandshakeResponse(NetworkEvent::Packet& packet, ENetPeer* peer);

    void onPlayerClick(NetworkEvent::Packet& packet, ENetPeer* peer);

  private:
    // Connections that are yet to be authorised
    std::vector<PendingClientSession> m_pendingConnections;

    // Authorised connected clients
    std::array<ClientSession, MAX_CONNECTIONS> m_clients;

    // Maps ENetPeer incomingPeerID to m_clients index
    std::unordered_map<uint32_t, unsigned> m_clientsMap;

    NetworkHost m_host;

    uint32_t m_salt;
};