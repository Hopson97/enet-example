#include "Client.h"

Client::Client()
    : m_host(2)
    , m_salt(generateSalt())

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
        std::cout << "Sending handshake to server.\n";
    }
    return isConnected;
}

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

bool Client::isConnected() const
{
    return m_connectState == ClientConnectState::Connected;
}

void Client::handlePacket(NetworkEvent::Packet& packet)
{
    using CTC = CommandToClient;

    // clang-format off
    switch (static_cast<CTC>(packet.command)) {
        case CTC::HandshakeChallenge:   onHandshakeChallenge    (packet);   break;
        case CTC::ConnectionAcceptance: onConnectionAcceptance  (packet);   break; 
    }
    // clang-format on
}

void Client::onHandshakeChallenge(NetworkEvent::Packet& packet)
{
    m_salt ^= packet.salt;
    auto response = makePacket(CommandToServer::HandshakeResponse, m_salt);
    m_serverConnection.send(response);
    std::cout << "Sending challenge response to server.\n";
}

void Client::onConnectionAcceptance(NetworkEvent::Packet& packet)
{
    uint8_t isAccecpted;
    packet.data >> isAccecpted;

    if (isAccecpted) {
        std::cout << "Connection accepted.\n";
    }
    else {
        std::string reason;
        packet.data >> reason;
        std::cerr << "Rejected connection: " << reason << ".\n";
    }
}

void Client::sendPlayerClick(float x, float y)
{
    auto packet = makePacket(CommandToServer::PlayerClick, m_salt);
    packet << m_playerId << x << y;
    m_serverConnection.send(packet);
}