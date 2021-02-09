#include "Client/Client.h"
#include "Common/World.h"
#include "Client/Keyboard.h"
#include "Server/Server.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <atomic>
#include <imgui/imgui.h>
#include <imgui_sfml/imgui-SFML.h>
#include <iostream>
#include <thread>
#include <vector>

constexpr int REGION_SIZE = 200;

// ================================================
//
//          S E R V E R   M A I N
//
// =================================================
int serverMain()
{
    std::atomic_bool isRunning{true};

    std::thread console([&]() {
        std::string line;
        while (isRunning) {
            std::cout << "Type a command.\n> ";
            std::getline(std::cin, line);
            if (line == "exit") {
                isRunning = false;
            }
        }
    });

    std::cout << "Starting server.\n";
    World world;
    Server server(world);
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        server.tick();
    }

    console.join();
    return 0;
}

// ================================================
//
//          C L I E N T   M A I N
//
// =================================================
int clientMain()
{
    World world;
    Client client(world);
    if (!client.connectTo(sf::IpAddress::getLocalAddress().toString())) {
        return -1;
    }

    // Set up the window and
    sf::RenderWindow window({REGION_SIZE * 7, REGION_SIZE * 5}, "SFML");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    // Start imgui
    ImGui::SFML::Init(window);

    // Set up some lines
    std::vector<sf::Vertex> regionLines;
    for (int y = 0; y < 5; y++) {
        sf::Vertex left;
        sf::Vertex right;

        left.position = {0, (float)(y * REGION_SIZE)};
        right.position = {(float)window.getSize().x, (float)(y * REGION_SIZE)};

        regionLines.push_back(left);
        regionLines.push_back(right);
    }
    for (int x = 0; x < 7; x++) {
        sf::Vertex left;
        sf::Vertex right;

        left.position = {(float)(x * REGION_SIZE), 0};
        right.position = {(float)(x * REGION_SIZE), (float)window.getSize().x};

        regionLines.push_back(left);
        regionLines.push_back(right);
    }

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

        window.draw(regionLines.data(), regionLines.size(), sf::Lines);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;

}

// ================================================
//
//                   M A I N
//
// =================================================
int main(int argc, char** argv)
{
    // Init ENET library and connect the client
    if (enet_initialize() != 0) {
        return EXIT_FAILURE;
    }

    // Convert command line into std::string
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        if (argc > 1) {
            std::cout << args.emplace_back(argv[i]) << std::endl;
        }
    }

    if (args.size() > 0) {
        if (args[0] == "client") {
            clientMain();
        }
        else if (args[0] == "server") {
            serverMain();
        }
        else if (args[0] == "both") {
            // BOTH????
        }
    }

    enet_deinitialize();
}