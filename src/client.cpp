#include "Keyboard.h"
#include "NetCommon.h"
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
    ENetHost* m_host = nullptr;
    ENetPeer* m_serverConnection = nullptr;

    ClientConnectState m_connectState = ClientConnectState::Pending;
};

Client::Client()
{
    m_host = enet_host_create(nullptr, 1, 1, 0, 0);
}

bool Client::connectTo(const std::string& ip)
{
    ENetAddress address;
    ENetEvent event;

    enet_address_set_host(&address, ip.c_str());
    address.port = 54321;

    m_serverConnection = enet_host_connect(m_host, &address, 1, 0);
    if (!m_serverConnection) {
        std::cerr << "Could not connect...\n";
        return false;
    }

    if (enet_host_service(m_host, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection succeeded.\n";
        return true;
    }
    else {
        enet_peer_reset(m_serverConnection);
        puts("Connection failed.");
        return false;
    }
}

void Client::tick()
{
    ENetEvent event;
    while (enet_host_service(m_host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
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

    ENetPacket* packet =
        enet_packet_create(p.getData(), p.getDataSize(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(m_serverConnection, 0, packet);
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