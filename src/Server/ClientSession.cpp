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