#include "Client.h"
#include "Keyboard.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <imgui/imgui.h>
#include <imgui_sfml/imgui-SFML.h>

int main()
{
    // Init ENET library and connect the client
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }
    Client client;
    if (!client.connectTo(sf::IpAddress::getLocalAddress().toString())) {
        return -1;
    }

    // Set up the window and
    sf::RenderWindow window({1280, 720}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    // Start imgui
    ImGui::SFML::Init(window);

    // Start the main game loop
    Keyboard keyboard;
    sf::Clock dt;
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            keyboard.update(e);
            ImGui::SFML::ProcessEvent(e);
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
        // Update
        client.tick();
        ImGui::SFML::Update(window, dt.restart());

        ImGui::ShowDemoWindow();

        if (!client.isConnected()) {
            window.close();
            break;
        }

        // Render
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    enet_deinitialize();
}