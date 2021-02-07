#pragma once

#include <SFML/Network/Packet.hpp>

#include <cstdint>

constexpr static uint16_t DEFAULT_PORT = 54123;

/**
 * @brief Network messages from the client sending to server
 */
enum class CommandToServer {
    /**
     *  Sends position of a click to the server
     *  Format:
     *      uint16_t playerId
     *      float x
     *      float y
     */
    PlayerClick
};

/**
 * @brief Network messages from the server sending to client
 */
enum class CommandToClient {

    /**
     * Sends an UUID to a newly connected player
     * Format:
     *   uint16_t playerId
     */
    PlayerId,

    /**
     *  Sends position of all player to the clients
     *  Format:
     *    uint16_t playerCount
     *    [playerCount]:
     *      uint16_t playerId
     *      float x
     *      float y
     */
    PlayerPositions

};

// Writes a command to a packet
template <typename CommandType>
sf::Packet& operator>>(sf::Packet& packet, CommandType& command)
{
    uint16_t commandId;
    packet >> commandId;
    command = static_cast<CommandType>(commandId);
    return packet;
}

// Reads a command from a packet
template <typename CommandType>
sf::Packet& operator<<(sf::Packet& packet, CommandType command)
{
    packet << static_cast<uint16_t>(command);
    return packet;
}
