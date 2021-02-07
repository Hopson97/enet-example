#include "ClientSession.h"

void PendingClientSession::sendHandshakeChallenge(uint32_t serversalt)
{
    auto packet = makePacket(CommandToClient::HandshakeChallenge, serversalt);
    connection.send(packet, ENET_PACKET_FLAG_RELIABLE);
}

void PendingClientSession::sendRejectConnection(const char* reason)
{
    auto packet = makePacket(CommandToClient::ConnectionAcceptance, 0);
    packet << static_cast<uint8_t>(0);
    packet << std::string{reason};
    connection.send(packet, ENET_PACKET_FLAG_RELIABLE);
}

void PendingClientSession::sendAcceptConnection(uint32_t playerId)
{
    auto packet = makePacket(CommandToClient::ConnectionAcceptance, 0);
    packet << static_cast<uint8_t>(1);
    packet << playerId;
    connection.send(packet, ENET_PACKET_FLAG_RELIABLE);
}

// ==========================================
//
//        C L I E N T    S E S S I O N
//
// ==========================================

void ClientSession::init(ENetPeer* peer, uint32_t salt, uint32_t playerId)
{
    m_salt = salt;
    m_clientConnection.handle = peer;
    m_playerId = playerId;
    m_isActive = true;
}

void ClientSession::disconnect()
{
    if (m_isActive) {
        enet_peer_disconnect(m_clientConnection.handle, 0);
        m_isActive = false;
    }
}

bool ClientSession::verify(uint32_t salt) const
{
    return salt == m_salt;
}

uint32_t ClientSession::getPlayerId() const
{
    return m_playerId;
}

bool ClientSession::isActive() const
{
    return m_isActive;
}
