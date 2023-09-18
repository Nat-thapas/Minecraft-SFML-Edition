#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include "mc_chunks.hpp"
#include "mc_chunk.hpp"
#include "mc_perfDebugInfo.hpp"
#include "idiv.hpp"
#include "mod.hpp"

int main() {
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(1600, 960), "I hate C++", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(0);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(false);

    mc::Chunks chunks(1, 64, 1600, 960, "resources/textures/atlases/blocksAtlas.png", "resources/textures/atlases/blocksAtlas.json");

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    mc::PerfDebugInfo perfDebugInfo(sf::Vector2f(0.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    sf::Clock elapsedClock;
    sf::Clock frameTimeClock;
    sf::Time frameTime;

    int playerChunkID = 1;

    float playerMoveSpeed = 4.317f;  // Blocks per second
    sf::Vector2i playerMoveDir(0, 0);
    sf::Vector2f playerPos(0.f, 192.f);

    bool playerLeftClick = false;
    bool playerRightClick = false;

    chunks.setPlayerPos(playerPos);

    int elapsedTime = elapsedClock.getElapsedTime().asMilliseconds();
    int lastTickTime = elapsedTime;
    
    while (window.isOpen())
    {
        perfDebugInfo.startFrame();

        frameTime = frameTimeClock.restart();
        elapsedTime = elapsedClock.getElapsedTime().asMilliseconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            playerMoveDir.y--;
                            break;
                        case sf::Keyboard::A:
                            playerMoveDir.x--;
                            break;
                        case sf::Keyboard::S:
                            playerMoveDir.y++;
                            break;
                        case sf::Keyboard::D:
                            playerMoveDir.x++;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            playerMoveDir.y++;
                            break;
                        case sf::Keyboard::A:
                            playerMoveDir.x++;
                            break;
                        case sf::Keyboard::S:
                            playerMoveDir.y--;
                            break;
                        case sf::Keyboard::D:
                            playerMoveDir.x--;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        playerLeftClick = true;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        playerRightClick = true;
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        playerLeftClick = false;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        playerRightClick = false;
                    }
                    break;
                default:
                    break;
            }
        }

        perfDebugInfo.endEventLoop();

        playerPos.x += playerMoveDir.x * playerMoveSpeed * frameTime.asSeconds();
        playerPos.y += playerMoveDir.y * playerMoveSpeed * frameTime.asSeconds();

        if (playerPos.x >= 16) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID++;
        }
        if (playerPos.x < 0) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID--;
        }

        chunks.setPlayerChunkID(playerChunkID);
        chunks.setPlayerPos(playerPos);

        chunks.setMouseScreenPos(sf::Mouse::getPosition(window));

        if (playerLeftClick) {
            chunks.breakBlock();
        }

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        if (elapsedTime - lastTickTime >= 50) {
            lastTickTime += 50;
            chunks.tickAnimation();
        }

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(160, 192, 255));
        window.draw(chunks);

        perfDebugInfo.endChunksRendering();
        perfDebugInfo.endEntitiesRendering();
        perfDebugInfo.endParticlesRendering();

        window.draw(perfDebugInfo);

        perfDebugInfo.endOverlaysRendering();

        window.display();

        perfDebugInfo.endFrame();
        perfDebugInfo.updateLabels();
    }

    return 0;
}