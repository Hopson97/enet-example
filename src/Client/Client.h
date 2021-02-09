#pragma once

#include "../Network/NetworkCommands.h"
#include "../Network/NetworkHost.h"
#include <enet/enet.h>

struct World;

enum class ClientConnectState {
    Pending,
    Connected,
    Disconnected,
};

class Client {
  public:
    Client(World& world);

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

    void onPlayerJoin(NetworkEvent::Packet& packet);
    void onForceExit(NetworkEvent::Packet& packet);
    void onPlayerPositions(NetworkEvent::Packet& packet);

  public:
    PlayerId_t m_playerId;

  private:
    NetworkHost m_host;
    NetworkConnection m_serverConnection;

    ClientConnectState m_connectState = ClientConnectState::Pending;

    // Used for auth stuff
    uint32_t m_salt;

    // World/ Game stuff
    World& mp_world;
};