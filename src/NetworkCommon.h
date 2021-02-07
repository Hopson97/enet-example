#pragma once

#include <SFML/Network/Packet.hpp>

#include <cstdint>

constexpr static uint16_t DEFAULT_PORT = 54123;

using NetworkCommand_t = uint16_t;

/**
 * @brief Network messages from the client sending to server
 */
enum class CommandToServer : NetworkCommand_t {
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
enum class CommandToClient : NetworkCommand_t {

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