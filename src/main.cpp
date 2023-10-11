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

#define MENU_NONE 0
#define MENU_PAUSE 1
#define MENU_SETTINGS 2
#define MENU_PLAYERINV 3
#define MENU_CRAFTINGTABLE 4
#define MENU_CHEST 5

using json = nlohmann::json;

int main() {
    srand(time(NULL));

    sf::ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 8;

    sf::Image cursorImage;
    cursorImage.loadFromFile("resources/textures/cursor.png");

    sf::Cursor crossCursor;
    crossCursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(cursorImage.getSize().x, cursorImage.getSize().y), sf::Vector2u(cursorImage.getSize().x / 2 - 1, cursorImage.getSize().y / 2 - 1));

    sf::Cursor arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);

    sf::FloatRect screenRect(0.f, 0.f, 1600.f, 900.f);

    sf::RenderWindow window(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "Minecraft SFML Edition", sf::Style::Default, ctxSettings);
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);
    window.setKeyRepeatEnabled(false);

    sf::Image icon;
    icon.loadFromFile("resources/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    int initialPlayerChunkID = 16;
    int pixelPerBlock = 32;

    mc::Chunks chunks(initialPlayerChunkID, 123654789, pixelPerBlock, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "resources/textures/atlases/", "resources/textures/atlases/", "resources/shaders/chunk.frag");

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
    mc::Inventory heldInventory(1, 1, uiScaling, 0, robotoMonoRegular, invTextureAtlas, invAtlasData);

    mainInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - mainInventory.getLocalBounds().width / 2.f, screenRect.height / 2.f - mainInventory.getLocalBounds().height / 2.f));

    sf::Texture hotbarInventoryTexture;
    hotbarInventoryTexture.loadFromFile("resources/textures/gui/hotbar.png");
    sf::Sprite hotbarInventorySprite(hotbarInventoryTexture);

    sf::Texture selectedHotbarSlotTexture;
    selectedHotbarSlotTexture.loadFromFile("resources/textures/gui/selectedHotbarSlot.png");
    sf::Sprite selectedHotbarSlotSprite(selectedHotbarSlotTexture);

    sf::Texture mainInventoryTexture;
    mainInventoryTexture.loadFromFile("resources/textures/gui/inventory.png");
    sf::Sprite mainInventorySprite(mainInventoryTexture);
    mainInventorySprite.setOrigin(sf::Vector2f(mainInventorySprite.getLocalBounds().width / 2.f, mainInventorySprite.getLocalBounds().height / 2.f));

    sf::RectangleShape menuBlackOutBackground;
    menuBlackOutBackground.setPosition(sf::Vector2f(0.f, 0.f));
    menuBlackOutBackground.setFillColor(sf::Color(7, 7, 7, 191));

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
    int openMenuType = 0;

    bool isFirstLoop = true;
    bool isFullScreen = false;

    while (window.isOpen()) {
        perfDebugInfo.startFrame();

        frameTime = frameTimeClock.restart();
        elapsedTime = elapsedClock.getElapsedTime();

        bool leftClick = false;
        bool rightClick = false;

        bool resized = isFirstLoop;
        bool ppbChanged = isFirstLoop;
        bool scalingChanged = isFirstLoop;
        bool menuChanged = isFirstLoop;

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
                        case sf::Keyboard::E:
                            menuChanged = true;
                            switch (openMenuType) {
                                case MENU_NONE:
                                    openMenuType = MENU_PLAYERINV;
                                    break;
                                case MENU_PLAYERINV:
                                case MENU_CRAFTINGTABLE:
                                case MENU_CHEST:
                                    openMenuType = MENU_NONE;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case sf::Keyboard::I:
                            ppbChanged = true;
                            pixelPerBlock *= 2;
                            break;
                        case sf::Keyboard::O:
                            ppbChanged = true;
                            pixelPerBlock /= 2;
                            break;
                        case sf::Keyboard::K:
                            scalingChanged = true;
                            uiScaling += 1;
                            break;
                        case sf::Keyboard::L:
                            scalingChanged = true;
                            uiScaling -= 1;
                            break;
                        case sf::Keyboard::F3:
                            displayDebug ^= 1;
                            break;
                        case sf::Keyboard::F11:
                            isFullScreen ^= 1;
                            if (isFullScreen) {
                                sf::VideoMode fullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
                                window.create(fullScreenVideoMode, "Minecraft SFML Edition", sf::Style::Default | sf::Style::Fullscreen, ctxSettings);
                                resized = true;
                                screenRect = sf::FloatRect(0, 0, fullScreenVideoMode.width, fullScreenVideoMode.height);
                                window.setView(sf::View(screenRect));
                            } else {
                                screenRect = sf::FloatRect(0.f, 0.f, 1600.f, 900.f);
                                window.create(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "Minecraft SFML Edition", sf::Style::Default, ctxSettings);
                                resized = true;
                                window.setView(sf::View(screenRect));
                            }
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
                            leftClick = true;
                            leftClickHeld = true;
                            break;
                        case sf::Mouse::Right:
                            rightClick = true;
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
                    resized = true;
                    screenRect = sf::FloatRect(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(screenRect));
                    break;
                default:
                    break;
            }
        }

        perfDebugInfo.endEventLoop();

        playerMoveInput = std::clamp(playerMoveInput, -1, 1);
        pixelPerBlock = std::clamp(pixelPerBlock, 1, 256);
        uiScaling = std::clamp(uiScaling, 1, 16);

        if (resized) {
            chunks.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
            player.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
            gameDebugInfo.setPosition(sf::Vector2f(screenRect.width - 5.f, 0.f));
        }

        if (ppbChanged) {
            chunks.setPixelPerBlock(pixelPerBlock);
            player.setPixelPerBlock(pixelPerBlock);
        }

        if (scalingChanged) {
            hotbarInventory.setScaling(uiScaling);
            mainInventory.setScaling(uiScaling);
            mainInventorySprite.setScale(sf::Vector2f(uiScaling, uiScaling));
            heldInventory.setScaling(uiScaling);
            heldInventory.setOrigin(sf::Vector2f(heldInventory.getSlotLocalBounds(0).width / 2.f, heldInventory.getSlotLocalBounds(0).height / 2.f));
        }

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

        gameDebugInfo.setPlayerLightLevel(chunks.getPlayerLightLevel());
        gameDebugInfo.setMouseLightLevel(chunks.getMouseLightLevel());
        
        switch (openMenuType) {
            case MENU_NONE:
                // TODO Drop held item
                break;
            case MENU_PLAYERINV:
                if (leftClick) {
                    for (int i = 0; i < 9; i++) {
                        if (hotbarInventory.getSlotGlobalBounds(i).contains(sf::Vector2f(mousePosition))) {
                            mc::ItemStack tempItemStack(hotbarInventory.getItemStack(i));
                            hotbarInventory.setItemStack(i, heldInventory.getItemStack(0));
                            heldInventory.setItemStack(0, tempItemStack);
                        }
                    }
                    for (int i = 0; i < 27; i++) {
                        if (mainInventory.getSlotGlobalBounds(i).contains(sf::Vector2f(mousePosition))) {
                            mc::ItemStack tempItemStack(mainInventory.getItemStack(i));
                            mainInventory.setItemStack(i, heldInventory.getItemStack(0));
                            heldInventory.setItemStack(0, tempItemStack);
                        }
                    }
                }
                break;
            default:
                break;
        }

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        if (elapsedTime.asMilliseconds() - lastTickTimeMs >= 50) {
            lastTickTimeMs += 50 + std::max(idiv(elapsedTime.asMilliseconds() - lastTickTimeMs, 50) - 100, 0) * 50;
            if (leftClickHeld && openMenuType == MENU_NONE) {
                int droppedItemID = chunks.breakBlock(player.xp);
                mc::ItemStack itemStack(1, droppedItemID);
                itemStack = hotbarInventory.addItemStack(itemStack);
                if (itemStack.amount > 0) itemStack = mainInventory.addItemStack(itemStack);
                if (itemStack.amount > 0) {
                    // TODO Drop item on the ground
                }
            }

            if (rightClickHeld && openMenuType == MENU_NONE) {
                if (chunks.placeBlock(hotbarInventory.getItemStack(selectedHotbarSlot).id)) {
                    hotbarInventory.subtractItem(selectedHotbarSlot, 1);
                }
            }

            // TODO chunks.update();
            chunks.tick(tickCount);
            chunks.updateVertexArrays();
            chunks.updateAnimatedVertexArrays();

            tickCount++;
        }

        chunks.updateLightLevels();
        chunks.updateLightingVertexArray();

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(122, 168, 255));
        window.draw(chunks);

        perfDebugInfo.endChunksRendering();

        window.draw(player);

        perfDebugInfo.endEntitiesRendering();
        perfDebugInfo.endParticlesRendering();

        if (resized || scalingChanged || menuChanged) {
            switch (openMenuType) {
                case MENU_NONE:
                    hotbarInventory.setMargin(2);
                    hotbarInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - hotbarInventory.getLocalBounds().width / 2.f, screenRect.height - hotbarInventory.getLocalBounds().height));
                    hotbarInventorySprite.setPosition(sf::Vector2f(hotbarInventory.getGlobalBounds().left - static_cast<float>(uiScaling), hotbarInventory.getGlobalBounds().top - static_cast<float>(uiScaling)));
                    hotbarInventorySprite.setScale(sf::Vector2f(uiScaling, uiScaling));
                    selectedHotbarSlotSprite.setScale(sf::Vector2f(uiScaling, uiScaling));
                    window.setMouseCursor(crossCursor);
                    if (heldInventory.getItemStack(0).id != 0) {
                        // TODO Drop item to ground
                    }
                    break;
                case MENU_PLAYERINV:
                    mainInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                    mainInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(uiScaling)));
                    hotbarInventory.setMargin(1);
                    hotbarInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(uiScaling)));
                    menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                    window.setMouseCursor(arrowCursor);
                    break;
                case MENU_PAUSE:
                case MENU_SETTINGS:
                case MENU_CRAFTINGTABLE:
                case MENU_CHEST:
                default:
                    break;
            }
        }
        
        selectedHotbarSlotSprite.setPosition(sf::Vector2f(hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).left - static_cast<float>(uiScaling) * 2.f, hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).top - static_cast<float>(uiScaling) * 2.f));

        switch (openMenuType) {
            case MENU_NONE:
                window.draw(hotbarInventorySprite);
                window.draw(selectedHotbarSlotSprite);
                window.draw(hotbarInventory);
                break;
            case MENU_PLAYERINV:
                heldInventory.setPosition(sf::Vector2f(mousePosition));
                window.draw(menuBlackOutBackground);
                window.draw(mainInventorySprite);
                window.draw(mainInventory);
                window.draw(hotbarInventory);
                window.draw(heldInventory);
                break;
            case MENU_PAUSE:
            case MENU_SETTINGS:
            case MENU_CRAFTINGTABLE:
            case MENU_CHEST:
            default:
                break;
        }

        if (displayDebug) {
            window.draw(gameDebugInfo);

            gameDebugInfo.updateLabels();
            window.draw(perfDebugInfo);
        }

        perfDebugInfo.endOverlaysRendering();

        window.display();

        perfDebugInfo.endFrame();
        perfDebugInfo.updateLabels();

        isFirstLoop = false;
    }

    return 0;
}