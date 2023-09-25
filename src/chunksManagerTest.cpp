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

    int playerChunkID = 1;

    mc::Chunks chunks(playerChunkID, 12365478, 16, sf::Vector2i(lround(screenRect.width), lround(screenRect.height)), "resources/textures/atlases/", "resources/textures/atlases/");

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    mc::PerfDebugInfo perfDebugInfo(sf::Vector2f(5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    mc::GameDebugInfo gameDebugInfo(sf::Vector2f(screenRect.width - 5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    sf::Clock elapsedClock;
    sf::Clock frameTimeClock;
    sf::Time frameTime;

    float playerMoveSpeed = 4.317f;  // Blocks per second
    float gravity = 25.f;
    sf::Vector2i playerMoveDir(0, 0);
    sf::Vector2f playerPos(0.f, 192.f);

    sf::Vector2i mousePosition(sf::Mouse::getPosition(window));

    bool leftClickHeld = false;
    bool rightClickHeld = false;

    chunks.setPlayerPos(playerPos);

    int elapsedTime = elapsedClock.getElapsedTime().asMilliseconds();
    int lastTickTime = elapsedTime;

    int xp = 0;
    int tickCount = 0;
    int pixelPerBlock = 16;
    bool displayDebug = false;
    float playerFallSpeed = 0.f;
    bool playerSprinting = false;
    float playerJump = 0.f;

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
                            if (playerJump <= 0.f) {
                                playerJump = 0.1f;
                            }
                            break;
                        case sf::Keyboard::A:
                            playerMoveDir.x--;
                            break;
                        case sf::Keyboard::D:
                            playerMoveDir.x++;
                            break;
                        case sf::Keyboard::LControl:
                            playerSprinting = true;
                            break;
                        case sf::Keyboard::Space:
                            if (playerJump <= 0.f) {
                                playerJump = 0.1f;
                            }
                            break;
                        case sf::Keyboard::I:
                            pixelPerBlock *= 2;
                            break;
                        case sf::Keyboard::O:
                            pixelPerBlock /= 2;
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
                            break;
                        case sf::Keyboard::A:
                            playerMoveDir.x++;
                            break;
                        case sf::Keyboard::D:
                            playerMoveDir.x--;
                            break;
                        case sf::Keyboard::LControl:
                            playerSprinting = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseMoved:
                    mousePosition = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
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

        pixelPerBlock = std::clamp(pixelPerBlock, 1, 256);

        chunks.setPixelPerBlock(pixelPerBlock);
        chunks.setScreenSize(sf::Vector2i(lround(screenRect.width), lround(screenRect.height)));

        playerPos.x += playerMoveDir.x * playerMoveSpeed * frameTime.asSeconds() * (1.f + 0.3f * playerSprinting);

        if (playerPos.x >= 16) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID++;
        }
        if (playerPos.x < 0) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID--;
        }

        if ((chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y)) - 2)) || (chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y)) - 1))) {
            if (playerMoveDir.x < 0) {
                playerPos.x += 1.f - mod(playerPos.x, 1.f);
            } else {
                playerPos.x -= mod(playerPos.x, 1.f);
            }
        }

        if (playerPos.x >= 16) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID++;
        }
        if (playerPos.x < 0) {
            playerPos.x = mod(playerPos.x, 16);
            playerChunkID--;
        }

        if (!chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y)))) {
            playerFallSpeed += gravity * frameTime.asSeconds();
            playerPos.y += std::min(playerFallSpeed * frameTime.asSeconds(), 0.9f);
        }

        if (chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y)))) {
            playerPos.y -= mod(playerPos.y, 1.f);
            playerFallSpeed = 0.f;
        }

        if ((playerJump > 0.f && chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y))) && !chunks.getBlock(playerChunkID, static_cast<int>(std::floor(playerPos.x)), static_cast<int>(std::floor(playerPos.y)) - 3)) || (playerJump < 0.1f && playerJump > 0.f)) {
            playerPos.y -= playerJump * playerJump * frameTime.asSeconds() * 3750.f;
            playerFallSpeed = 0.f;
            playerJump -= frameTime.asSeconds();
        } else {
            playerJump = 0.f;
        }

        std::cout << playerJump << std::endl;
        chunks.setPlayerChunkID(playerChunkID);
        chunks.setPlayerPos(playerPos);

        gameDebugInfo.setPlayerChunkID(playerChunkID);
        gameDebugInfo.setPlayerPos(playerPos);

        gameDebugInfo.setLoadedChunks(chunks.getLoadedChunks());

        chunks.setMouseScreenPos(mousePosition);

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