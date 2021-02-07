#pragma once

#include "ClientSession.h"
#include "NetworkHost.h"
#include <cstdint>
#include <enet/enet.h>
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

    void tick();

  private:
    void onClientConnect(ENetPeer* peer);
    void onClientDisconnect(ENetPeer* peer);

    void handlePacket(NetworkEvent::Packet& packet, ENetPeer* peer);

    void onHandshake(NetworkEvent::Packet& packet, ENetPeer* peer);
    void onHandshakeResponse(NetworkEvent::Packet& packet, ENetPeer* peer);

  private:
    std::vector<PendingClientSession> m_pendingConnections;

    std::vector<Player> m_players;
    // ENetHost* m_host;

    NetworkHost m_host;

    // Used for auth stuff
    uint32_t m_salt;
};