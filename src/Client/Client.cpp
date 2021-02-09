#include "Client.h"

#include "../Network/NetworkCommands.h"
#include "../World/World.h"
#include <iostream>

Client::Client(World& world)
    : m_host(2)
    , m_salt(generateSalt())
    , mp_world(world)

{
}

Client::~Client()
{
    disconnect();
}

void Client::disconnect()
{
    if (m_connectState != ClientConnectState::Disconnected) {
        if (m_host.disconnectClient(m_serverConnection)) {
            m_connectState = ClientConnectState::Disconnected;
        }
    }
}

bool Client::connectTo(const std::string& ip)
{
    bool isConnected = m_host.connectTo(ip.c_str(), m_serverConnection);
    if (isConnected) {
        m_connectState = ClientConnectState::Pending;

        auto handshake = makePacket(CommandToServer::Handshake, m_salt);
        m_serverConnection.send(handshake);
    }
    return isConnected;
}

bool Client::isConnected() const
{
    return m_connectState != ClientConnectState::Disconnected;
}

// ================================================
//
//          NETWORK EVENT HANDLING
//
// =================================================
void Client::tick()
{
    NetworkEvent event;
    while (m_host.pollEvent(event)) {
        switch (event.type) {
            case NetworkEventType::Data:
                handlePacket(event.packet);
                enet_packet_destroy(event.enetPacket);
                break;

            default:
                break;
        }
    }
}

// ================================================
//
//          PACKET SENDING/ BROADCASTING
//
// =================================================
void Client::sendPlayerClick(float x, float y)
{
    auto packet = makePacket(CommandToServer::PlayerClick, m_salt);
    packet << m_playerId << x << y;
    m_serverConnection.send(packet);
}

// ================================================
//
//          PACKET HANDLING FUNCTIONS
//
// =================================================
void Client::handlePacket(NetworkEvent::Packet& packet)
{
    using CTC = CommandToClient;

    // clang-format off
    switch (static_cast<CTC>(packet.command)) {
        case CTC::HandshakeChallenge:   onHandshakeChallenge    (packet);   break;
        case CTC::ConnectionAcceptance: onConnectionAcceptance  (packet);   break; 

        case CTC::PlayerJoined:         onPlayerJoin        (packet);   break; 
        case CTC::ForceExit:            onForceExit         (packet);   break; 
        case CTC::PlayerPositions:      onPlayerPositions   (packet);   break; 
    }
    // clang-format on
}

void Client::onHandshakeChallenge(NetworkEvent::Packet& packet)
{
    m_salt ^= packet.salt;
    std::cout << "Salt: " << m_salt << std::endl;
    auto response = makePacket(CommandToServer::HandshakeResponse, m_salt);
    m_serverConnection.send(response);
}

void Client::onConnectionAcceptance(NetworkEvent::Packet& packet)
{
    uint8_t isAccecpted;
    packet.data >> isAccecpted;

    if (isAccecpted) {
        std::cout << "Connection accepted.\n";
        packet.data >> m_playerId;
    }
    else {
        std::string reason;
        packet.data >> reason;
        std::cerr << "Rejected connection: " << reason << ".\n";
    }
}

void Client::onPlayerJoin(NetworkEvent::Packet& packet)
{
    uint16_t id;
    packet.data >> id;
    std::cout << "Player joined with ID " << id << ".\n";
}

void Client::onForceExit(NetworkEvent::Packet& packet)
{
    std::string reason;
    packet.data >> reason;
    std::cout << "Force exit recieved. Reason: " << reason << "\n";
    m_connectState = ClientConnectState::Disconnected;
}

void Client::onPlayerPositions(NetworkEvent::Packet& packet)
{
    uint16_t count;

    packet.data >> count;
}