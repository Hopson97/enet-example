#pragma once

#include "../Common/NetworkHost.h"

/**
 * @brief A pending client session as stored on the server
 *
 */
struct PendingClientSession {
    NetworkConnection connection;
    uint32_t salt = 0;

    void sendHandshakeChallenge(uint32_t serversalt);
    void sendAcceptConnection(uint32_t playerId);
    void sendRejectConnection(const char* reason);
};

class ClientSession {
  public:
    void init(ENetPeer* peer, uint32_t salt, uint32_t playerId);
    void disconnect();

  private:
    NetworkConnection m_clientConnection;
    uint32_t m_salt = 0;
    uint32_t m_playerId = 0;
};