#pragma once

#include "../Common/NetworkCommon.h"
#include "../Common/NetworkHost.h"
#include <enet/enet.h>
#include <iostream>

enum class ClientConnectState {
    Pending,
    Connected,
    Disconnected,
};

class Client {
  public:
    Client();

    Client(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    ~Client();

    /// @brief Disconnect the client from the server
    void disconnect();

    bool connectTo(const std::string& ip);
    void tick();
    bool isConnected() const;

    void sendPlayerClick(float x, float y);

  private:
    void handlePacket(NetworkEvent::Packet& packet);

    void onHandshakeChallenge(NetworkEvent::Packet& packet);
    void onConnectionAcceptance(NetworkEvent::Packet& packet);

  public:
    uint16_t m_playerId;

  private:
    NetworkHost m_host;
    NetworkConnection m_serverConnection;

    ClientConnectState m_connectState = ClientConnectState::Pending;

    // Used for auth stuff
    uint32_t m_salt;
};