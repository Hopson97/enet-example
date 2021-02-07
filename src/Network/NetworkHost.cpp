#include "NetworkHost.h"

#include "NetworkCommon.h"
#include "NetworkPacket.h"
#include <cassert>
#include <ctime>
#include <iostream>
#include <random>

namespace {
    uint32_t generateSalt()
    {
        std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
        std::uniform_int_distribution<uint32_t> dist(0, 4294967290);
        return dist(rng);
    }
} // namespace

// Creates a server host
NetworkHost::NetworkHost(unsigned maxConnections, unsigned channels)
    : salt(generateSalt())
{
    ENetAddress address;

    address.host = ENET_HOST_ANY;
    address.port = DEFAULT_PORT;

    m_handle = enet_host_create(&address, maxConnections, channels, 0, 0);
    if (!m_handle) {
        std::cerr << "Could not create server.\n";
    }
}

// Creates a client host
NetworkHost::NetworkHost(unsigned channels)
    : salt(generateSalt())
{
    m_handle = enet_host_create(nullptr, 1, channels, 0, 0);
    if (!m_handle) {
        std::cerr << "Could not create client.\n";
    }
}

NetworkHost::~NetworkHost()
{
    if (m_handle) {
        enet_host_destroy(m_handle);
    }
}

bool NetworkHost::pollEvent(NetworkEvent& event, unsigned timeout)
{
    assert(m_handle);
    if (enet_host_service(m_handle, &event.handle, timeout)) {
        event.type = static_cast<NetworkEventType>(event.handle.type);
        event.peer = event.handle.peer;
        event.enetPacket = event.handle.packet;

        if (event.type == NetworkEventType::Data) {
            event.packet.data.append(event.handle.packet->data,
                                     event.handle.packet->dataLength + 1);
            event.packet.data >> event.packet.command;
        }
        return true;
    }
    return false;
}

bool NetworkHost::connectTo(const char* ipAddress, NetworkConnection& outConnection)
{
    assert(m_handle);

    // Create address for the client to connect to
    ENetAddress address{};
    address.port = DEFAULT_PORT;
    if (enet_address_set_host(&address, ipAddress) != 0) {
        std::cerr << "Failed to create address.";
        return false;
    }

    // Connect to the server
    outConnection.handle = enet_host_connect(m_handle, &address, 2, 0);
    if (!outConnection.handle) {
        std::cerr << "Failed to connect to the server.";
        return false;
    }

    // Wait for a connection establishment
    bool connected = [this] {
        ENetEvent event;
        while (enet_host_service(m_handle, &event, 2000) > 0) {
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                enet_packet_destroy(event.packet);
            }
            else if (event.type == ENET_EVENT_TYPE_CONNECT) {
                return true;
            }
        }
        return false;
    }();
    if (!connected) {
        std::cerr << "Failed to establish connection with the server.";
        return false;
    }
    return true;
}

bool NetworkHost::disconnectClient(NetworkConnection& serverConnection)
{
    assert(m_handle);
    assert(serverConnection.handle);
    enet_peer_disconnect(serverConnection.handle, 0);

    ENetEvent event;
    while (enet_host_service(m_handle, &event, 2000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            enet_host_flush(m_handle);
            return true;
        }
    }
    enet_peer_reset(serverConnection.handle);
    return false;
}

void NetworkConnection::send(const sf::Packet& p, uint32_t flags, unsigned channel)
{
    assert(handle);
    ENetPacket* packet = enet_packet_create(p.getData(), p.getDataSize(), flags);
    enet_peer_send(handle, channel, packet);
}