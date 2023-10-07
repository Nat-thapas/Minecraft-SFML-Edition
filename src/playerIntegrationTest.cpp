#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mc_chunk.hpp"
#include "mc_chunks.hpp"
#include "mc_player.hpp"
#include "mc_inventory.hpp"
#include "mc_gameDebugInfo.hpp"
#include "mc_perfDebugInfo.hpp"
#include "mod.hpp"

using json = nlohmann::json;

int main() {
    srand(time(NULL));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::Image cursorImage;
    cursorImage.loadFromFile("resources/textures/cursor.png");

    sf::Cursor cursor;
    cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(cursorImage.getSize().x, cursorImage.getSize().y), sf::Vector2u(cursorImage.getSize().x / 2 - 1, cursorImage.getSize().y / 2 - 1));

    sf::FloatRect screenRect(0.f, 0.f, 1600.f, 900.f);

    sf::RenderWindow window(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "I hate C++", sf::Style::Default, settings);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursor(cursor);

    int initialPlayerChunkID = 1;
    int pixelPerBlock = 32;

    mc::Chunks chunks(initialPlayerChunkID, 123654789, pixelPerBlock, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "resources/textures/atlases/", "resources/textures/atlases/");

    sf::Vector2f initialPlayerPos(0.5f, 0.f);
    while (!chunks.getBlock(initialPlayerChunkID, static_cast<int>(initialPlayerPos.x), static_cast<int>(initialPlayerPos.y))) {
        initialPlayerPos.y++;
    }

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    sf::Font robotoMonoRegular;
    robotoMonoRegular.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");

    mc::PerfDebugInfo perfDebugInfo(sf::Vector2f(5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    mc::GameDebugInfo gameDebugInfo(sf::Vector2f(screenRect.width - 5.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    sf::Clock elapsedClock;
    sf::Clock frameTimeClock;
    sf::Time elapsedTime;
    sf::Time frameTime;

    // friction, drag = speed * coefficient
    float playerMaxSpeed = 4.317f;  // block/second
    float playerFromStillAcceleration = 40.f;  // m/s^2
    float playerTerminalVelocity = 80.f;  // m/s
    float playerMass = 75.f;  // kg
    float playerMovementForce = playerFromStillAcceleration * playerMass;  // F = ma; N
    float gravity = 25.f;  // m/s^2
    float playerFrictionCoefficient = playerMovementForce / playerMaxSpeed;
    float playerAirDragCoefficient = playerMass * gravity / playerTerminalVelocity;

    mc::Player player(chunks, initialPlayerChunkID, initialPlayerPos, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), pixelPerBlock, "resources/textures/players/right.png", playerMovementForce, playerMass, gravity, playerFrictionCoefficient, playerAirDragCoefficient);

    int playerMoveInput = 0;
    bool playerIntendJump = false;

    sf::Texture invTextureAtlas;
    invTextureAtlas.loadFromFile("resources/textures/atlases/itemsAtlas.png");

    std::ifstream invAtlasDataFile("resources/textures/atlases/itemsAtlas.json");
    json invAtlasData = json::parse(invAtlasDataFile);

    int uiScaling = 2;

    mc::Inventory hotbarInventory(9, 9, uiScaling, 2, robotoMonoRegular, invTextureAtlas, invAtlasData);
    mc::Inventory mainInventory(27, 9, uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData);

    hotbarInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - hotbarInventory.getLocalBounds().width / 2.f, screenRect.height - hotbarInventory.getLocalBounds().height));
    mainInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - mainInventory.getLocalBounds().width / 2.f, screenRect.height / 2.f - mainInventory.getLocalBounds().height / 2.f));

    sf::Texture hotbarInventoryTexture;
    hotbarInventoryTexture.loadFromFile("resources/textures/gui/hotbar.png");
    sf::Sprite hotbarInventorySprite(hotbarInventoryTexture);

    sf::Texture selectedHotbarSlotTexture;
    selectedHotbarSlotTexture.loadFromFile("resources/textures/gui/selectedHotbarSlot.png");
    sf::Sprite selectedHotbarSlotSprite(selectedHotbarSlotTexture);

    float scrollWheelFraction = 0.f;
    int selectedHotbarSlot = 0;

    sf::Vector2i mousePosition(sf::Mouse::getPosition(window));

    bool leftClickHeld = false;
    bool rightClickHeld = false;

    chunks.setPlayerPos(player.getPosition());

    elapsedTime = elapsedClock.getElapsedTime();
    int lastTickTimeMs = elapsedTime.asMilliseconds();

    int tickCount = 0;
    bool displayDebug = false;
    bool playerSprinting = false;

    while (window.isOpen()) {
        perfDebugInfo.startFrame();

        frameTime = frameTimeClock.restart();
        elapsedTime = elapsedClock.getElapsedTime();

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
                        case sf::Keyboard::Space:
                            playerIntendJump = true;
                            break;
                        case sf::Keyboard::A:
                            playerMoveInput--;
                            break;
                        case sf::Keyboard::D:
                            playerMoveInput++;
                            break;
                        case sf::Keyboard::LControl:
                            playerSprinting = true;
                            break;
                        case sf::Keyboard::I:
                            pixelPerBlock *= 2;
                            break;
                        case sf::Keyboard::O:
                            pixelPerBlock /= 2;
                            break;
                        case sf::Keyboard::K:
                            uiScaling += 1;
                            break;
                        case sf::Keyboard::L:
                            uiScaling -= 1;
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
                        case sf::Keyboard::Space:
                            playerIntendJump = false;
                            break;
                        case sf::Keyboard::A:
                            playerMoveInput++;
                            break;
                        case sf::Keyboard::D:
                            playerMoveInput--;
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
                    switch (event.mouseButton.button) {
                        case sf::Mouse::Left:
                            leftClickHeld = true;
                            break;
                        case sf::Mouse::Right:
                            rightClickHeld = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    switch (event.mouseButton.button) {
                        case sf::Mouse::Left:
                            leftClickHeld = false;
                            break;
                        case sf::Mouse::Right:
                            rightClickHeld = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseWheelScrolled:
                    selectedHotbarSlot -= static_cast<int>(std::floor(event.mouseWheelScroll.delta));
                    scrollWheelFraction += event.mouseWheelScroll.delta - std::floor(event.mouseWheelScroll.delta);
                    if (abs(scrollWheelFraction) >= 1.f) {
                        selectedHotbarSlot -= static_cast<int>(std::floor(scrollWheelFraction));
                        scrollWheelFraction -= std::floor(scrollWheelFraction);
                    }
                    selectedHotbarSlot = mod(selectedHotbarSlot, 9);
                    break;
                case sf::Event::Resized:
                    screenRect = sf::FloatRect(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(screenRect));
                    chunks.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
                    player.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
                    gameDebugInfo.setPosition(sf::Vector2f(screenRect.width - 5.f, 0.f));
                    break;
                default:
                    break;
            }
        }

        perfDebugInfo.endEventLoop();

        playerMoveInput = std::clamp(playerMoveInput, -1, 1);
        pixelPerBlock = std::clamp(pixelPerBlock, 1, 256);
        uiScaling = std::clamp(uiScaling, 1, 16);

        hotbarInventory.setScaling(uiScaling);
        mainInventory.setScaling(uiScaling);

        hotbarInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - hotbarInventory.getLocalBounds().width / 2.f, screenRect.height - hotbarInventory.getLocalBounds().height));
        mainInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - mainInventory.getLocalBounds().width / 2.f, screenRect.height / 2.f - mainInventory.getLocalBounds().height / 2.f));

        chunks.setPixelPerBlock(pixelPerBlock);
        player.setPixelPerBlock(pixelPerBlock);

        hotbarInventory.setScaling(uiScaling);
        mainInventory.setScaling(uiScaling);

        hotbarInventorySprite.setPosition(sf::Vector2f(hotbarInventory.getGlobalBounds().left - static_cast<float>(uiScaling), hotbarInventory.getGlobalBounds().top - static_cast<float>(uiScaling)));
        hotbarInventorySprite.setScale(sf::Vector2f(uiScaling, uiScaling));

        selectedHotbarSlotSprite.setPosition(sf::Vector2f(hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).left - static_cast<float>(uiScaling) * 2.f, hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).top - static_cast<float>(uiScaling) * 2.f));
        selectedHotbarSlotSprite.setScale(sf::Vector2f(uiScaling, uiScaling));

        if (playerIntendJump) {
            player.jump();
        }
        player.setLateralForce(playerMoveInput, playerSprinting);
        player.update(frameTime);

        chunks.setPlayerChunkID(player.getChunkID());
        chunks.setPlayerPos(player.getPosition());

        gameDebugInfo.setPlayerChunkID(player.getChunkID());
        gameDebugInfo.setPlayerPos(player.getPosition());
        gameDebugInfo.setPlayerVelocity(player.getVelocity());

        gameDebugInfo.setLoadedChunks(chunks.getLoadedChunks());

        chunks.setMouseScreenPos(mousePosition);

        gameDebugInfo.setMouseChunkID(chunks.getMouseChunkID());
        gameDebugInfo.setMousePos(chunks.getMousePos());

        gameDebugInfo.setPlayerLightLevel(sf::Vector2i(0, 0));
        gameDebugInfo.setMouseLightLevel(sf::Vector2i(0, 0));

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        int droppedItemID;

        if (leftClickHeld) {
            droppedItemID = chunks.breakBlock(player.xp);
            mc::ItemStack itemStack(1, droppedItemID);
            hotbarInventory.addItemStack(itemStack);
        }

        if (rightClickHeld) {
            if (chunks.placeBlock(hotbarInventory.getItemStack(selectedHotbarSlot).id)) {
                hotbarInventory.subtractItem(selectedHotbarSlot, 1);
            }
        }

        if (elapsedTime.asMilliseconds() - lastTickTimeMs >= 50) {
            lastTickTimeMs += 50 + std::max(idiv(elapsedTime.asMilliseconds() - lastTickTimeMs, 50) - 100, 0) * 50;
            chunks.tick(tickCount);
            tickCount++;
        }

        chunks.updateVertexArrays();

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(160, 192, 255));
        window.draw(chunks);

        perfDebugInfo.endChunksRendering();

        window.draw(player);

        perfDebugInfo.endEntitiesRendering();
        perfDebugInfo.endParticlesRendering();

        window.draw(hotbarInventorySprite);
        window.draw(selectedHotbarSlotSprite);

        window.draw(hotbarInventory);
        window.draw(mainInventory);

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