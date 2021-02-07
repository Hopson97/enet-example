#pragma once

#include <enet/enet.h>
#include <string>
#include <SFML/Network/Packet.hpp>

enum class NetworkEventType { Connection, Timeout, Disconnection, Data };

struct NetworkEvent {
    ENetEvent handle;
    NetworkEventType type;

    ENetPacket* packet;
    ENetPeer* peer;
};

struct NetworkConnection {
    ENetPeer* handle = nullptr;
    uint32_t salt;

    void send(const sf::Packet& packet, unsigned channel = 0, uint32_t flags = 0);
};

class NetworkHost {
  public:
    /// @brief Construct a new Network Host object as a server host
    /// @param maxConnections The maxium number of allowed connections
    /// @param channels The number of communication channels
    NetworkHost(unsigned maxConnections, unsigned channels);

    /// @brief Construct a new Network Host object as a client object
    /// @param channels The number of communication channels
    NetworkHost(unsigned channels);

    /// @brief Poll network events into the event object, eg connections, packet recieves etc
    /// @param event Objecto put the event details into
    /// @return true An event was received
    /// @return false No event was received
    bool pollEvent(NetworkEvent& event);

    /// @brief Connects the client host to a server
    /// @param ipAddress Server IP Address
    /// @param outConnection The server connection that will be created
    /// @return true Connection was a success
    /// @return false Connection was a failure
    bool connectTo(const char* ipAddress, NetworkConnection& outConnection);

  private:
    ENetHost* m_handle = nullptr;
};