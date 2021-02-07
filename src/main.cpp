#include "Network/Client.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Keyboard.h"

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