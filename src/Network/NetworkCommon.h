#pragma once

#include <SFML/Network/Packet.hpp>

#include <cstdint>
#include <ctime>
#include <random>

constexpr static uint16_t DEFAULT_PORT = 54123;

using NetworkCommand_t = uint16_t;

// Connection process:

// Client joins
// Server accepts
// Client sends random salt value (Handshake part 1)
// Server recieves salt (Handshake part 1), stores salt value for that connection, sends
// salt back (handshake challenge) client recieves it server salt, XORs it with its own,
// sends it back (handshake response) server tests if the XOR value is the same, if it is
// then it sends accept connection, and game data (given game nit full)

/**
 * @brief Network messages from the client sending to server
 */
enum class CommandToServer : NetworkCommand_t {
    // Sends client salt to the server
    // -- Data --
    // u32: Client salt value
    Handshake,

    // Sends client salt to the server combined with the server's salt
    // -- Data --
    // u32: Client salt value
    HandshakeResponse,

    // Sends position of a click to the server
    // - Data -
    // u16: playerId
    // f32: x
    // f32: y
    PlayerClick
};

/**
 * @brief Network messages from the server sending to client
 */
enum class CommandToClient : NetworkCommand_t {
    // Send handshake response to the client
    // -- Data --
    // u32: The salt sent from the client
    // u32: The server's salt
    HandshakeChallenge,

    // Sends reject or accept to a pending connection.
    // -- Data --
    // u8: accept (0 for no, 1 for yes)
    // (IF ACCPET)
    // u16 playerId
    ConnectionAcceptance,

    // Sends position of all player to the clients
    // -- Data --
    // u16: playerCount
    // [for each playerCount]:
    // u16: playerId
    // f32: x
    // f32: y
    //PlayerPositions
};

inline uint32_t generateSalt()
{
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<uint32_t> dist(0, 4294967290);
    return dist(rng);
}