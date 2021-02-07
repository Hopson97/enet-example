#include "Keyboard.h"
#include "NetworkCommon.h"
#include "NetworkHost.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <array>
#include <enet/enet.h>
#include <iostream>

enum class ClientConnectState {
    Pending,
    Connected,
    Disconnected,
};

class Client {
  public:
    Client();

    Client(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) = delete;

    ~Client();

    /// @brief Disconnect the client from the server
    void disconnect();

    bool connectTo(const std::string& ip);
    void tick();
    bool isConnected() const;

    void sendPlayerClick(float x, float y);

  private:
    void handlePacket(ENetPacket* packet);

    void onPlayerId(sf::Packet& packet);
    void onPlayerPositions(sf::Packet& packet);

  public:
    uint16_t m_playerId;

  private:
    NetworkHost m_host;
    NetworkConnection m_serverConnection;

    ClientConnectState m_connectState = ClientConnectState::Pending;
};

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
                enet_packet_destroy(event.packet);
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

void Client::handlePacket(ENetPacket* packet)
{
    sf::Packet p;
    p.append(packet->data, packet->dataLength + 1);

    CommandToClient cmd;
    p >> cmd;
    // clang-format off
    switch (cmd) {
        case CommandToClient::PlayerId:         onPlayerId(p);          break;
        case CommandToClient::PlayerPositions:  onPlayerPositions(p);   break; 
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
    sf::Packet p;
    p << CommandToServer::PlayerClick << m_playerId << x << y;
    m_serverConnection.send(p);
}

int main()
{
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    Client client;
    if (!client.connectTo("192.168.0.20")) {
        return -1;
    }

    sf::RenderWindow window({1280, 720}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    Keyboard keyboard;

    sf::Clock dt;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            keyboard.update(e);
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonReleased:
                    switch (e.mouseButton.button) {
                        case sf::Mouse::Button::Left:
                            client.sendPlayerClick(e.mouseButton.x, e.mouseButton.y);
                            break;

                        default:
                            break;
                    }

                default:
                    break;
            }
        }

        client.tick();

        window.clear();
        window.display();
    }

    enet_deinitialize();
}