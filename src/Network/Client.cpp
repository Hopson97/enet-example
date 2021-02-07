#include "Client.h"

Client::Client()
    : m_host(2)
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
    return m_host.connectTo(ip.c_str(), m_serverConnection);
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
    sf::Packet& data = packet.data;

    // clang-format off
    switch (static_cast<CommandToClient>(packet.command)) {
        case CommandToClient::PlayerId:         onPlayerId(data);          break;
        case CommandToClient::PlayerPositions:  onPlayerPositions(data);   break; 
    }
    // clang-format on
}

void Client::onPlayerId(sf::Packet& packet)
{
    packet >> m_playerId;
    m_connectState = ClientConnectState::Connected;
    printf("Got player ID! %d", m_playerId);
}

void Client::onPlayerPositions(sf::Packet& packet)
{
    uint16_t count = 0;
    packet >> count;
}

void Client::sendPlayerClick(float x, float y)
{
    auto packet = makePacket(CommandToServer::PlayerClick);
    packet << m_playerId << x << y;
    m_serverConnection.send(packet);
}