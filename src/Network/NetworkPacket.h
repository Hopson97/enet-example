#pragma once

#include "NetworkCommon.h"
#include "enet/enet.h"
#include <SFML/Network/Packet.hpp>
#include <cstdint>

template <typename Command>
sf::Packet makePacket(Command command, uint32_t salt)
{
    sf::Packet packet;
    packet << command << salt;
    return packet;
}

// Writes a command to a packet
template <typename CommandType>
sf::Packet& operator>>(sf::Packet& packet, CommandType& command)
{
    NetworkCommand_t commandId;
    packet >> commandId;
    command = static_cast<CommandType>(commandId);
    return packet;
}

// Reads a command from a packet
template <typename CommandType>
sf::Packet& operator<<(sf::Packet& packet, CommandType command)
{
    packet << static_cast<NetworkCommand_t>(command);
    return packet;
}
