#include "NetworkHost.h"

#include <iostream>
#include "NetCommon.h"

// Creates a server host
NetworkHost::NetworkHost(unsigned maxConnections, unsigned channels)
{
    ENetAddress address;

    address.host = ENET_HOST_ANY;
    address.port = 54321;

    m_handle = enet_host_create(&address, maxConnections, channels, 0, 0);
    if (!m_handle) {
        std::cerr << "Could not create server.\n";
    }
}

// Creates a client host
NetworkHost::NetworkHost(unsigned channels)
{
    m_handle = enet_host_create(nullptr, 1, channels, 0, 0);
    if (!m_handle) {
        std::cerr << "Could not create client.\n";
    }
}


bool NetworkHost::pollEvent(NetworkEvent& event)
{
    if (enet_host_service(m_handle, &event.handle, 0)) {
        event.type = static_cast<NetworkEventType>(event.handle.type);
        event.packet = event.handle.packet;
        event.peer = event.handle.peer;
        return true;
    }
    return false;
}

bool NetworkHost::connectTo(const char* ipAddress, NetworkConnection& outConnection)
{
    if (!m_handle) {
        std::cerr << "The host is not created, unable to connect the client to a server.\n";
        return false;
    }
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