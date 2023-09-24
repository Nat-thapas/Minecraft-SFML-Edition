#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>

#include "idiv.hpp"
#include "mc_chunk.hpp"
#include "mc_chunks.hpp"
#include "mc_gameDebugInfo.hpp"
#include "mc_perfDebugInfo.hpp"
#include "mod.hpp"

int main() {
    srand(time(NULL));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::Image cursorImage;
    cursorImage.loadFromFile("resources/textures/cursor.png");

    sf::Cursor cursor;
    cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(cursorImage.getSize().x, cursorImage.getSize().y), sf::Vector2u(cursorImage.getSize().x / 2 - 1, cursorImage.getSize().y / 2 - 1));

    sf::FloatRect screenRect(0.f, 0.f, 1600.f, 900.f);

    sf::RenderWindow window(sf::VideoMode(lround(screenRect.width), lround(screenRect.height)), "I hate C++", sf::Style::Default, settings);
    window.setFramerateLimit(0);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursor(cursor);

    mc::Chunks chunks(1, 12365478, 16, sf::Vector2i(lround(screenRect.width), lround(screenRect.height)), "resources/textures/atlases/", "resources/textures/atlases/");

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    mc::PerfDebugInfo perfDebugInfo(sf::Vector2f(5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    mc::GameDebugInfo gameDebugInfo(sf::Vector2f(screenRect.width - 5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    sf::Clock elapsedClock;
    sf::Clock frameTimeClock;
    sf::Time frameTime;

    int playerChunkID = 1;

    float playerMoveSpeed = 4.317f * 10.f;  // Blocks per second
    sf::Vector2i playerMoveDir(0, 0);
    sf::Vector2f playerPos(0.f, 192.f);

    bool leftClickHeld = false;
    bool rightClickHeld = false;

    chunks.setPlayerPos(playerPos);

    int elapsedTime = elapsedClock.getElapsedTime().asMilliseconds();
    int lastTickTime = elapsedTime;

    int xp = 0;
    int tickCount = 0;
    int pixelPerBlock = 16;
    bool displayDebug = false;

    while (window.isOpen()) {
        perfDebugInfo.startFrame();

        frameTime = frameTimeClock.restart();
        elapsedTime = elapsedClock.getElapsedTime().asMilliseconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    chunks.saveAll();
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
                        case sf::Keyboard::I:
                            pixelPerBlock *= 2;
                            break;
                        case sf::Keyboard::O:
                            if (pixelPerBlock > 1) {
                                pixelPerBlock /= 2;
                            }
                            break;
                        case sf::Keyboard::F3:
                            displayDebug ^= 1;
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
                        leftClickHeld = true;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        rightClickHeld = true;
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        leftClickHeld = false;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        rightClickHeld = false;
                    }
                    break;
                case sf::Event::Resized:
                    screenRect = sf::FloatRect(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(screenRect));
                    gameDebugInfo.setPosition(sf::Vector2f(screenRect.width - 5.f, 0.f));
                    break;
                default:
                    break;
            }
        }

        perfDebugInfo.endEventLoop();

        chunks.setPixelPerBlock(pixelPerBlock);
        chunks.setScreenSize(sf::Vector2i(lround(screenRect.width), lround(screenRect.height)));

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

        gameDebugInfo.setPlayerChunkID(playerChunkID);
        gameDebugInfo.setPlayerPos(playerPos);

        gameDebugInfo.setLoadedChunks(chunks.getLoadedChunks());

        chunks.setMouseScreenPos(sf::Mouse::getPosition(window));

        gameDebugInfo.setMouseChunkID(chunks.getMouseChunkID());
        gameDebugInfo.setMousePos(chunks.getMousePos());

        gameDebugInfo.setPlayerLightLevel(sf::Vector2i(0, 0));
        gameDebugInfo.setMouseLightLevel(sf::Vector2i(0, 0));

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        if (leftClickHeld) {
            chunks.breakBlock(xp);
        }

        if (elapsedTime - lastTickTime >= 50) {
            lastTickTime += 50;
            chunks.tick(tickCount);
            tickCount++;
        }

        chunks.updateVertexArrays();

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(160, 192, 255));
        window.draw(chunks);

        perfDebugInfo.endChunksRendering();
        perfDebugInfo.endEntitiesRendering();
        perfDebugInfo.endParticlesRendering();

        if (displayDebug) {
            window.draw(gameDebugInfo);

            gameDebugInfo.updateLabels();
            window.draw(perfDebugInfo);
        }

        perfDebugInfo.endOverlaysRendering();

        window.display();

        perfDebugInfo.endFrame();
        perfDebugInfo.updateLabels();
    }

    return 0;
}