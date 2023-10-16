#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mc_chunk.hpp"
#include "mc_chunks.hpp"
#include "mc_craftingInterface.hpp"
#include "mc_gameDebugInfo.hpp"
#include "mc_inventory.hpp"
#include "mc_musicPlayer.hpp"
#include "mc_perfDebugInfo.hpp"
#include "mc_player.hpp"
#include "mod.hpp"

#define MENU_NONE 0
#define MENU_PAUSE 1
#define MENU_SETTINGS 2
#define MENU_PLAYERINV 3
#define MENU_CRAFTINGTABLE 4
#define MENU_CHEST 5
#define MENU_FURNACE 6

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

    std::string worldName = "test";
    int initialPlayerChunkID = 16;
    int pixelPerBlock = 32;

    mc::Chunks chunks(initialPlayerChunkID, 123654789, pixelPerBlock, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "resources/textures/atlases/", "resources/textures/atlases/", "resources/shaders/chunk.frag");

    sf::Vector2f initialPlayerPos(0.5f, 0.f);
    while (!chunks.getBlock(initialPlayerChunkID, static_cast<int>(initialPlayerPos.x), static_cast<int>(initialPlayerPos.y))) {
        initialPlayerPos.y++;
    }

    std::vector<std::string> musicNames = {"calm1.ogg", "calm2.ogg", "calm3.ogg", "hal1.ogg", "hal2.ogg", "hal3.ogg", "hal4.ogg", "piano1.ogg", "piano2.ogg", "piano3.ogg", "an_ordinary_day.ogg", "comforting_memories.ogg", "floating_dream.ogg", "infinite_amethyst.ogg", "left_to_bloom.ogg", "one_more_day.ogg", "stand_tall.ogg", "wedding.ogg", "creative1.ogg", "creative2.ogg", "creative3.ogg", "creative4.ogg", "creative5.ogg", "creative6.ogg"};
    mc::MusicPlayer musicPlayer("resources/sounds/music/game/", musicNames);
    musicPlayer.start();

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
    float playerMaxSpeed = 4.317f;                                         // block/second
    float playerFromStillAcceleration = 40.f;                              // m/s^2
    float playerTerminalVelocity = 80.f;                                   // m/s
    float playerMass = 75.f;                                               // kg
    float playerMovementForce = playerFromStillAcceleration * playerMass;  // F = ma; N
    float gravity = 25.f;                                                  // m/s^2
    float playerFrictionCoefficient = playerMovementForce / playerMaxSpeed;
    float playerAirDragCoefficient = playerMass * gravity / playerTerminalVelocity;

    mc::Player player(chunks, initialPlayerChunkID, initialPlayerPos, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), pixelPerBlock, "resources/textures/players/right.png", playerMovementForce, playerMass, gravity, playerFrictionCoefficient, playerAirDragCoefficient);

    int playerMoveInput = 0;
    bool playerIntendJump = false;

    sf::Texture invTextureAtlas;
    invTextureAtlas.loadFromFile("resources/textures/atlases/itemsAtlas.png");

    std::ifstream invAtlasDataFile("resources/textures/atlases/itemsAtlas.json");
    json invAtlasData = json::parse(invAtlasDataFile);

    int uiScaling = 3;

    mc::Inventory hotbarInventory(9, 9, uiScaling, 2, robotoMonoRegular, invTextureAtlas, invAtlasData);
    mc::Inventory mainInventory(27, 9, uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData);
    mc::Inventory heldInventory(1, 1, uiScaling, 0, robotoMonoRegular, invTextureAtlas, invAtlasData);

    mc::Inventory chestInventory(27, 9, uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData);

    std::ifstream recipesDataFile("resources/recipes/crafting.json");
    json recipesData = json::parse(recipesDataFile);

    mc::CraftingInterface crafting2x2_inventory(2, uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData, recipesData);
    mc::CraftingInterface crafting3x3_table(3, uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData, recipesData);

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

    sf::Texture chestInventoryTexture;
    chestInventoryTexture.loadFromFile("resources/textures/gui/chest.png");
    sf::Sprite chestInventorySprite(chestInventoryTexture);
    chestInventorySprite.setOrigin(sf::Vector2f(chestInventorySprite.getLocalBounds().width / 2.f, chestInventorySprite.getLocalBounds().height / 2.f));

    sf::Texture craftingTableInventoryTexture;
    craftingTableInventoryTexture.loadFromFile("resources/textures/gui/crafting_table.png");
    sf::Sprite craftingTableInventorySprite(craftingTableInventoryTexture);
    craftingTableInventorySprite.setOrigin(sf::Vector2f(craftingTableInventorySprite.getLocalBounds().width / 2.f, craftingTableInventorySprite.getLocalBounds().height / 2.f));

    sf::RectangleShape inventorySlotHoverHighlighter;
    inventorySlotHoverHighlighter.setFillColor(sf::Color(255, 255, 255, 127));

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

    bool unsavedChestEdit = false;
    int openedChestChunkID = 0;
    sf::Vector2i openedChestPos(0, 0);

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

        bool renderSlotHoverHighlighter = false;

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
                                case MENU_FURNACE:
                                    openMenuType = MENU_NONE;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case sf::Keyboard::Escape:
                            menuChanged = true;
                            switch (openMenuType) {
                                case MENU_PLAYERINV:
                                case MENU_CRAFTINGTABLE:
                                case MENU_CHEST:
                                case MENU_FURNACE:
                                    openMenuType = MENU_NONE;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case sf::Keyboard::Num1:
                            selectedHotbarSlot = 0;
                            break;
                        case sf::Keyboard::Num2:
                            selectedHotbarSlot = 1;
                            break;
                        case sf::Keyboard::Num3:
                            selectedHotbarSlot = 2;
                            break;
                        case sf::Keyboard::Num4:
                            selectedHotbarSlot = 3;
                            break;
                        case sf::Keyboard::Num5:
                            selectedHotbarSlot = 4;
                            break;
                        case sf::Keyboard::Num6:
                            selectedHotbarSlot = 5;
                            break;
                        case sf::Keyboard::Num7:
                            selectedHotbarSlot = 6;
                            break;
                        case sf::Keyboard::Num8:
                            selectedHotbarSlot = 7;
                            break;
                        case sf::Keyboard::Num9:
                            selectedHotbarSlot = 8;
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
            chestInventorySprite.setScale(sf::Vector2f(uiScaling, uiScaling));
            craftingTableInventorySprite.setScale(sf::Vector2f(uiScaling, uiScaling));
            crafting2x2_inventory.setScaling(uiScaling);
            crafting3x3_table.setScaling(uiScaling);
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
                if (unsavedChestEdit) {
                    std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}", worldName, openedChestChunkID, openedChestPos.x, openedChestPos.y);
                    chestInventory.saveToFile(filePath);
                    chestInventory.clear();
                    unsavedChestEdit = false;
                }
                if (rightClick && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 39) {
                    menuChanged = true;
                    openMenuType = MENU_CRAFTINGTABLE;
                }
                if (rightClick && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 40) {
                    menuChanged = true;
                    std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}", worldName, chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y);
                    if (std::filesystem::exists(filePath)) {
                        chestInventory.loadFromFile(filePath);
                    } else {
                        chestInventory.clear();
                    }
                    openedChestChunkID = chunks.getMouseChunkID();
                    openedChestPos = chunks.getMousePos();
                    unsavedChestEdit = true;
                    openMenuType = MENU_CHEST;
                }
                break;
            case MENU_PLAYERINV:
                // Hotbar
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = hotbarInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, hotbarInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(hotbarInventory.getItemStack(i));
                                hotbarInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(hotbarInventory.getItemStack(i).id, hotbarInventory.getItemStack(i).amount / 2 + hotbarInventory.getItemStack(i).amount % 2);
                                hotbarInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (hotbarInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || hotbarInventory.getItemStack(i).id == 0)) {
                                hotbarInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Main inv
                for (int i = 0; i < 27; i++) {
                    sf::FloatRect bound = mainInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, mainInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(mainInventory.getItemStack(i));
                                mainInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(mainInventory.getItemStack(i).id, mainInventory.getItemStack(i).amount / 2 + mainInventory.getItemStack(i).amount % 2);
                                mainInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (mainInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || mainInventory.getItemStack(i).id == 0)) {
                                mainInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // 2x2 crafting
                for (int i = 0; i < 4; i++) {
                    sf::FloatRect bound = crafting2x2_inventory.getInputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (crafting2x2_inventory.getInputItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, crafting2x2_inventory.addInputItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(crafting2x2_inventory.getInputItemStack(i));
                                crafting2x2_inventory.setInputItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(crafting2x2_inventory.getInputItemStack(i).id, crafting2x2_inventory.getInputItemStack(i).amount / 2 + crafting2x2_inventory.getInputItemStack(i).amount % 2);
                                crafting2x2_inventory.subtractInputItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (crafting2x2_inventory.getInputEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (crafting2x2_inventory.getInputItemStack(i).id == heldInventory.getItemStack(0).id || crafting2x2_inventory.getInputItemStack(i).id == 0)) {
                                crafting2x2_inventory.addInputItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // 2x2 crafting output
                for (int i = 0; i < 1; i++) {
                    sf::FloatRect bound = crafting2x2_inventory.getOutputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick && (heldInventory.getItemStack(0).id == 0 || (heldInventory.getItemStack(0).id == crafting2x2_inventory.getOutputItemStack(0).id && heldInventory.getEmptySpace(0) >= crafting2x2_inventory.getOutputItemStack(0).amount))) {
                            heldInventory.addItemStack(0, crafting2x2_inventory.takeOutputItem(0));
                        }
                    }
                }
                break;
            case MENU_CHEST:
                // Hotbar
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = hotbarInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, hotbarInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(hotbarInventory.getItemStack(i));
                                hotbarInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(hotbarInventory.getItemStack(i).id, hotbarInventory.getItemStack(i).amount / 2 + hotbarInventory.getItemStack(i).amount % 2);
                                hotbarInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (hotbarInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || hotbarInventory.getItemStack(i).id == 0)) {
                                hotbarInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Main inv
                for (int i = 0; i < 27; i++) {
                    sf::FloatRect bound = mainInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, mainInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(mainInventory.getItemStack(i));
                                mainInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(mainInventory.getItemStack(i).id, mainInventory.getItemStack(i).amount / 2 + mainInventory.getItemStack(i).amount % 2);
                                mainInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (mainInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || mainInventory.getItemStack(i).id == 0)) {
                                mainInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Chest inventory
                for (int i = 0; i < 27; i++) {
                    sf::FloatRect bound = chestInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (chestInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, chestInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(chestInventory.getItemStack(i));
                                chestInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(chestInventory.getItemStack(i).id, chestInventory.getItemStack(i).amount / 2 + chestInventory.getItemStack(i).amount % 2);
                                chestInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (chestInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (chestInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || chestInventory.getItemStack(i).id == 0)) {
                                chestInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                break;
            case MENU_CRAFTINGTABLE:
                // Hotbar
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = hotbarInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, hotbarInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(hotbarInventory.getItemStack(i));
                                hotbarInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(hotbarInventory.getItemStack(i).id, hotbarInventory.getItemStack(i).amount / 2 + hotbarInventory.getItemStack(i).amount % 2);
                                hotbarInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (hotbarInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (hotbarInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || hotbarInventory.getItemStack(i).id == 0)) {
                                hotbarInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Main inv
                for (int i = 0; i < 27; i++) {
                    sf::FloatRect bound = mainInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, mainInventory.addItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(mainInventory.getItemStack(i));
                                mainInventory.setItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(mainInventory.getItemStack(i).id, mainInventory.getItemStack(i).amount / 2 + mainInventory.getItemStack(i).amount % 2);
                                mainInventory.subtractItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (mainInventory.getEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (mainInventory.getItemStack(i).id == heldInventory.getItemStack(0).id || mainInventory.getItemStack(i).id == 0)) {
                                mainInventory.addItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // 3x3 crafting
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = crafting3x3_table.getInputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (crafting3x3_table.getInputItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, crafting3x3_table.addInputItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(crafting3x3_table.getInputItemStack(i));
                                crafting3x3_table.setInputItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(crafting3x3_table.getInputItemStack(i).id, crafting3x3_table.getInputItemStack(i).amount / 2 + crafting3x3_table.getInputItemStack(i).amount % 2);
                                crafting3x3_table.subtractInputItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (crafting3x3_table.getInputEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (crafting3x3_table.getInputItemStack(i).id == heldInventory.getItemStack(0).id || crafting3x3_table.getInputItemStack(i).id == 0)) {
                                crafting3x3_table.addInputItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // 3x3 crafting output
                for (int i = 0; i < 1; i++) {
                    sf::FloatRect bound = crafting3x3_table.getOutputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * uiScaling, 1.f * uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * uiScaling, 2.f * uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick && (heldInventory.getItemStack(0).id == 0 || (heldInventory.getItemStack(0).id == crafting3x3_table.getOutputItemStack(0).id && heldInventory.getEmptySpace(0) >= crafting3x3_table.getOutputItemStack(0).amount))) {
                            heldInventory.addItemStack(0, crafting3x3_table.takeOutputItem(0));
                        }
                    }
                }
                break;
            default:
                break;
        }

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        // Drop items from inventories (furnaces, chests)
        if (elapsedTime.asMilliseconds() - lastTickTimeMs >= 50) {
            lastTickTimeMs += 50 + std::max(idiv(elapsedTime.asMilliseconds() - lastTickTimeMs, 50) - 100, 0) * 50;
            if (leftClickHeld && openMenuType == MENU_NONE) {
                int droppedItemID = chunks.breakBlock(player.xp);
                mc::ItemStack itemStack(droppedItemID, 1);
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
            chunks.update();
            chunks.updateVertexArrays();
            chunks.updateAnimatedVertexArrays();

            tickCount++;

            musicPlayer.update();
        }

        chunks.updateLightLevels();
        chunks.updateLightingVertexArray();

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(144, 184, 255));
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
                    crafting2x2_inventory.setInputPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 88.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 25.f * static_cast<float>(uiScaling)));
                    crafting2x2_inventory.setOutputPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 144.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 35.f * static_cast<float>(uiScaling)));
                    menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                    window.setMouseCursor(arrowCursor);
                    break;
                case MENU_PAUSE:
                    break;
                case MENU_SETTINGS:
                    break;
                case MENU_CRAFTINGTABLE:
                    craftingTableInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                    mainInventory.setPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(uiScaling)));
                    hotbarInventory.setMargin(1);
                    hotbarInventory.setPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(uiScaling)));
                    crafting3x3_table.setInputPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 29.f * static_cast<float>(uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 16.f * static_cast<float>(uiScaling)));
                    crafting3x3_table.setOutputPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 123.f * static_cast<float>(uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 34.f * static_cast<float>(uiScaling)));
                    menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                    window.setMouseCursor(arrowCursor);
                    break;
                case MENU_CHEST:
                    chestInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                    mainInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(uiScaling)));
                    hotbarInventory.setMargin(1);
                    hotbarInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), mainInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(uiScaling)));
                    chestInventory.setPosition(sf::Vector2f(chestInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(uiScaling), chestInventorySprite.getGlobalBounds().top + 17.f * static_cast<float>(uiScaling)));
                    menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                    window.setMouseCursor(arrowCursor);
                    break;
                case MENU_FURNACE:
                    break;
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
                window.draw(crafting2x2_inventory);
                if (renderSlotHoverHighlighter) window.draw(inventorySlotHoverHighlighter);
                window.draw(heldInventory);
                break;
            case MENU_PAUSE:
                break;
            case MENU_SETTINGS:
                break;
            case MENU_CRAFTINGTABLE:
                heldInventory.setPosition(sf::Vector2f(mousePosition));
                window.draw(menuBlackOutBackground);
                window.draw(craftingTableInventorySprite);
                window.draw(mainInventory);
                window.draw(hotbarInventory);
                window.draw(crafting3x3_table);
                if (renderSlotHoverHighlighter) window.draw(inventorySlotHoverHighlighter);
                window.draw(heldInventory);
                break;
            case MENU_CHEST:
                heldInventory.setPosition(sf::Vector2f(mousePosition));
                window.draw(menuBlackOutBackground);
                window.draw(chestInventorySprite);
                window.draw(mainInventory);
                window.draw(hotbarInventory);
                window.draw(chestInventory);
                if (renderSlotHoverHighlighter) window.draw(inventorySlotHoverHighlighter);
                window.draw(heldInventory);
                break;
            case MENU_FURNACE:
                break;
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