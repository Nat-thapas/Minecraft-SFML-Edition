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
#include "mc_button.hpp"
#include "mc_chunk.hpp"
#include "mc_chunks.hpp"
#include "mc_craftingInterface.hpp"
#include "mc_furnaceInterface.hpp"
#include "mc_gameDebugInfo.hpp"
#include "mc_inventory.hpp"
#include "mc_leaderboard.hpp"
#include "mc_locationDelta.hpp"
#include "mc_menuBackground.hpp"
#include "mc_musicPlayer.hpp"
#include "mc_perfDebugInfo.hpp"
#include "mc_preferences.hpp"
#include "mc_player.hpp"
#include "mc_slider.hpp"
#include "mc_soundEffect.hpp"
#include "mc_textBox.hpp"
#include "mod.hpp"

#define MENU_MAIN -10
#define MENU_SELECTWORLD -9
#define MENU_LEADERBOARD -8
#define MENU_NONE 0
#define MENU_PAUSE 1
#define MENU_SETTINGS 2
#define MENU_PLAYERINV 3
#define MENU_CRAFTINGTABLE 4
#define MENU_CHEST 5
#define MENU_FURNACE 6
#define MENU_ENTERNAME 7

 using json = nlohmann::json;

void game(std::string worldName, sf::RenderWindow& window, sf::FloatRect screenRect, mc::SoundEffect& soundEffect, mc::PreferencesData& preferences) {
    sf::ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = preferences.antialiasingLevel;

    sf::Image cursorImage;
    cursorImage.loadFromFile("resources/textures/cursor.png");

    sf::Cursor crossCursor;
    crossCursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(cursorImage.getSize().x, cursorImage.getSize().y), sf::Vector2u(cursorImage.getSize().x / 2 - 1, cursorImage.getSize().y / 2 - 1));

    sf::Cursor arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);

    window.setFramerateLimit(preferences.framerateLimit);
    window.setVerticalSyncEnabled(preferences.vsyncEnabled);
    window.setKeyRepeatEnabled(false);

    sf::Image icon;
    icon.loadFromFile("resources/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::Listener::setPosition(sf::Vector3f(0.f, 0.f, 0.f));
    sf::Listener::setDirection(sf::Vector3f(0.f, 0.f, -1.f));
    sf::Listener::setUpVector(sf::Vector3f(0.f, 1.f, 0.f));
    sf::Listener::setGlobalVolume(static_cast<float>(preferences.masterVolume));

    std::ifstream smeltingRecipesDataFile("resources/recipes/smelting.json");
    json smeltingRecipesData = json::parse(smeltingRecipesDataFile);
    smeltingRecipesDataFile.close();

    bool recalculateSpawnY = false;

    mc::PlayerLocationData playerLocationData;
    if (std::filesystem::exists(std::format("saves/{}/player.dat.gz", worldName))) {
        playerLocationData = mc::Player::getDataFromFile(std::format("saves/{}/player.dat.gz", worldName));
    } else {
        playerLocationData.chunkID = 0;
        recalculateSpawnY = true;
    }

    int seed;
    if (std::filesystem::exists(std::format("saves/{}/seed.dat", worldName))) {
        std::ifstream seedFile(std::format("saves/{}/seed.dat", worldName), std::ios::binary);
        seedFile.read(reinterpret_cast<char*>(&seed), sizeof(int));
        seedFile.close();
    } else {
        seed = ((rand() & 0b11111111) << 24) + ((rand() & 0b11111111) << 16) + ((rand() & 0b11111111) << 8) + (rand() & 0b11111111);
    }

    int playtime = 0;
    if (std::filesystem::exists(std::format("saves/{}/playtime.dat", worldName))) {
        std::ifstream playtimeFile(std::format("saves/{}/playtime.dat", worldName), std::ios::binary);
        playtimeFile.read(reinterpret_cast<char*>(&playtime), sizeof(int));
        playtimeFile.close();
    }

    mc::Chunks chunks(playerLocationData.chunkID, seed, preferences.gamePixelPerBlock, worldName, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "resources/textures/atlases/", "resources/textures/atlases/", "resources/shaders/chunk.frag", "resources/shaders/breakOverlay.frag", smeltingRecipesData, "resources/textures/overlays/breakProgress.png", soundEffect);

    if (recalculateSpawnY) {
        playerLocationData.position = sf::Vector2f(0.5f, 0.f);
        while (!chunks.getBlock(playerLocationData.chunkID, static_cast<int>(playerLocationData.position.x), static_cast<int>(playerLocationData.position.y))) {
            playerLocationData.position.y++;
        }
    }

    std::vector<std::string> musicNames = {"calm1.ogg", "calm2.ogg", "calm3.ogg", "hal1.ogg", "hal2.ogg", "hal3.ogg", "hal4.ogg", "piano1.ogg", "piano2.ogg", "piano3.ogg", "an_ordinary_day.ogg", "comforting_memories.ogg", "floating_dream.ogg", "infinite_amethyst.ogg", "left_to_bloom.ogg", "one_more_day.ogg", "stand_tall.ogg", "wedding.ogg", "creative1.ogg", "creative2.ogg", "creative3.ogg", "creative4.ogg", "creative5.ogg", "creative6.ogg"};
    mc::MusicPlayer musicPlayer("resources/sounds/music/game/", musicNames, static_cast<float>(preferences.musicVolume) / 100.f);
    musicPlayer.start();

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    sf::Font robotoMonoRegular;
    robotoMonoRegular.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");

    sf::Font sarabunRegular;
    sarabunRegular.loadFromFile("resources/fonts/Sarabun-Regular.ttf");

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

    mc::Player player(chunks, playerLocationData.chunkID, playerLocationData.position, sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), preferences.gamePixelPerBlock, "resources/textures/players/right.png", playerMovementForce, playerMass, gravity, playerFrictionCoefficient, playerAirDragCoefficient);

    int playerMoveInput = 0;
    bool playerIntendJump = false;

    bool diamondAcquired = std::filesystem::exists(std::format("saves/{}/diamondAcquired", worldName));

    sf::Texture invTextureAtlas;
    invTextureAtlas.loadFromFile("resources/textures/atlases/itemsAtlas.png");

    std::ifstream invAtlasDataFile("resources/textures/atlases/itemsAtlas.json");
    json invAtlasData = json::parse(invAtlasDataFile);
    invAtlasDataFile.close();

    mc::Inventory hotbarInventory(9, 9, preferences.uiScaling, 2, robotoMonoRegular, invTextureAtlas, invAtlasData);
    mc::Inventory mainInventory(27, 9, preferences.uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData);
    mc::Inventory heldInventory(1, 1, preferences.uiScaling, 0, robotoMonoRegular, invTextureAtlas, invAtlasData);

    std::string filePath = std::format("saves/{}/inventories/player/hotbar.dat.gz", worldName);
    if (std::filesystem::exists(filePath)) {
        hotbarInventory.loadFromFile(filePath);
    }

    filePath = std::format("saves/{}/inventories/player/main.dat.gz", worldName);
    if (std::filesystem::exists(filePath)) {
        mainInventory.loadFromFile(filePath);
    }

    mc::Inventory chestInventory(27, 9, preferences.uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData);

    std::ifstream recipesDataFile("resources/recipes/crafting.json");
    json recipesData = json::parse(recipesDataFile);
    recipesDataFile.close();

    mc::CraftingInterface crafting2x2_inventory(2, preferences.uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData, recipesData);
    mc::CraftingInterface crafting3x3_table(3, preferences.uiScaling, 1, robotoMonoRegular, invTextureAtlas, invAtlasData, recipesData);

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
    craftingTableInventoryTexture.loadFromFile("resources/textures/gui/craftingTable.png");
    sf::Sprite craftingTableInventorySprite(craftingTableInventoryTexture);
    craftingTableInventorySprite.setOrigin(sf::Vector2f(craftingTableInventorySprite.getLocalBounds().width / 2.f, craftingTableInventorySprite.getLocalBounds().height / 2.f));

    sf::Texture furnaceProgressBarTexture;
    furnaceProgressBarTexture.loadFromFile("resources/textures/gui/furnaceArrowIcon.png");

    sf::Texture furnaceFuelBarTexture;
    furnaceFuelBarTexture.loadFromFile("resources/textures/gui/furnacefireIcon.png");

    mc::FurnaceInterface furnaceInterface(preferences.uiScaling, robotoMonoRegular, invTextureAtlas, invAtlasData, smeltingRecipesData, furnaceProgressBarTexture, furnaceFuelBarTexture);

    sf::Texture furnaceInventoryTexture;
    furnaceInventoryTexture.loadFromFile("resources/textures/gui/furnace.png");
    sf::Sprite furnaceInventorySprite(furnaceInventoryTexture);
    furnaceInventorySprite.setOrigin(sf::Vector2f(furnaceInventorySprite.getLocalBounds().width / 2.f, furnaceInventorySprite.getLocalBounds().height / 2.f));

    sf::RectangleShape inventorySlotHoverHighlighter;
    inventorySlotHoverHighlighter.setFillColor(sf::Color(255, 255, 255, 127));

    sf::RectangleShape menuBlackOutBackground;
    menuBlackOutBackground.setPosition(sf::Vector2f(0.f, 0.f));
    menuBlackOutBackground.setFillColor(sf::Color(7, 7, 7, 191));

    sf::Texture shortButtonTexture;
    shortButtonTexture.loadFromFile("resources/textures/gui/buttonShort.png");
    sf::Texture mediumButtonTexture;
    mediumButtonTexture.loadFromFile("resources/textures/gui/buttonMedium.png");
    sf::Texture longButtonTexture;
    longButtonTexture.loadFromFile("resources/textures/gui/buttonLong.png");

    sf::Texture shortSliderTexture;
    shortSliderTexture.loadFromFile("resources/textures/gui/sliderShort.png");
    sf::Texture mediumSliderTexture;
    mediumSliderTexture.loadFromFile("resources/textures/gui/sliderMedium.png");
    sf::Texture longSliderTexture;
    longSliderTexture.loadFromFile("resources/textures/gui/sliderLong.png");
    sf::Texture slidingTexture;
    slidingTexture.loadFromFile("resources/textures/gui/sliderSliding.png");

    sf::Text gameMenuText;
    gameMenuText.setFont(robotoRegular);
    gameMenuText.setLetterSpacing(1.25f);
    gameMenuText.setCharacterSize(12 * preferences.uiScaling);
    gameMenuText.setFillColor(sf::Color::White);
    gameMenuText.setOutlineColor(sf::Color::Black);
    gameMenuText.setOutlineThickness(0.5f * preferences.uiScaling);
    gameMenuText.setString("Game Menu");
    mc::Button backToGameButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Back to Game");
    mc::Button optionsButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Options...");
    mc::Button saveAndQuitButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Save and Quit to Title");

    sf::Text optionText;
    optionText.setFont(robotoRegular);
    optionText.setLetterSpacing(1.25f);
    optionText.setCharacterSize(12 * preferences.uiScaling);
    optionText.setFillColor(sf::Color::White);
    optionText.setOutlineColor(sf::Color::Black);
    optionText.setOutlineThickness(0.5f * preferences.uiScaling);
    optionText.setString("Options");
    mc::Slider masterVolumeSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Volume: {}%", preferences.masterVolume), 100);
    mc::Slider musicVolumeSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Music: {}%", preferences.musicVolume), 100);
    mc::Slider antialiasingLevelSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Antialiasing: {}", preferences.antialiasingLevel), 8);
    mc::Slider framerateLimitSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, preferences.framerateLimit != 0 ? std::format("Max Framerate: {} fps", preferences.framerateLimit) : "Max Framerate: Unlimited", 25);
    mc::Button fullscreenToggleButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, preferences.fullScreenEnabled ? "Fullscreen: ON" : "Fullscreen: OFF");
    mc::Button vsyncToggleButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, preferences.vsyncEnabled ? "VSync: ON" : "VSync: OFF");
    mc::Button uiScalingStepButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, std::format("GUI Scale: {}", preferences.uiScaling));
    mc::Slider ppbSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock), 8);
    mc::Button settingsDoneButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Done");

    sf::Text enterNameText;
    enterNameText.setFont(robotoRegular);
    enterNameText.setLetterSpacing(1.25f);
    enterNameText.setCharacterSize(12 * preferences.uiScaling);
    enterNameText.setFillColor(sf::Color::White);
    enterNameText.setOutlineColor(sf::Color::Black);
    enterNameText.setOutlineThickness(0.5f * preferences.uiScaling);
    enterNameText.setString("Please Enter Your Name");
    mc::TextBox playerNameTextBox(sarabunRegular, sf::Vector2f(200.f, 20.f), preferences.uiScaling);
    mc::Button enterNameDoneButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Done");

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
    int openMenuType = MENU_NONE;

    bool isFirstLoop = true;

    bool unsavedChestEdit = false;
    int openedChestChunkID = 0;
    sf::Vector2i openedChestPos(0, 0);

    int openedFurnaceChunkID = 0;
    sf::Vector2i openedFurnacePos(0, 0);

    int lastPlaceTickCount = 0;

    while (window.isOpen()) {
        perfDebugInfo.startFrame();

        frameTime = frameTimeClock.restart();
        elapsedTime = elapsedClock.getElapsedTime();

        bool leftClick = false;
        bool rightClick = false;

        bool resized = isFirstLoop;
        bool ppbChanged = isFirstLoop;
        bool scalingChanged = isFirstLoop;
        bool volumeChanged = false;
        bool windowSettingsChanged = false;
        bool windowNeedRecreate = false;

        bool renderSlotHoverHighlighter = false;

        sf::String textInputed = "";

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: {
                    if (unsavedChestEdit) {
                        std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}.dat.gz", worldName, openedChestChunkID, openedChestPos.x, openedChestPos.y);
                        chestInventory.saveToFile(filePath);
                    }
                    chunks.saveAll();
                    std::ofstream seedFile(std::format("saves/{}/seed.dat", worldName), std::ios::binary);
                    seedFile.write(reinterpret_cast<char*>(&seed), sizeof(int));
                    seedFile.close();
                    playtime += tickCount;
                    std::ofstream playtimeFile(std::format("saves/{}/playtime.dat", worldName), std::ios::binary);
                    playtimeFile.write(reinterpret_cast<char*>(&playtime), sizeof(int));
                    playtimeFile.close();
                    hotbarInventory.saveToFile(std::format("saves/{}/inventories/player/hotbar.dat.gz", worldName));
                    mainInventory.saveToFile(std::format("saves/{}/inventories/player/main.dat.gz", worldName));
                    mc::Player::saveDataToFile(std::format("saves/{}/player.dat.gz", worldName), mc::PlayerLocationData(chunks.getPlayerChunkID(), chunks.getPlayerPos()));
                    mc::Preferences::saveToFile("settings.dat.gz", preferences);
                    window.close();
                    return;
                    break;
                }
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
                            switch (openMenuType) {
                                case MENU_PLAYERINV:
                                case MENU_CRAFTINGTABLE:
                                case MENU_CHEST:
                                case MENU_FURNACE:
                                case MENU_PAUSE:
                                    openMenuType = MENU_NONE;
                                    break;
                                case MENU_SETTINGS:
                                    openMenuType = MENU_PAUSE;
                                    break;
                                case MENU_NONE:
                                    openMenuType = MENU_PAUSE;
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
                            if (openMenuType == MENU_ENTERNAME) break;
                            ppbChanged = true;
                            preferences.gamePixelPerBlock *= 2;
                            break;
                        case sf::Keyboard::O:
                            if (openMenuType == MENU_ENTERNAME) break;
                            ppbChanged = true;
                            preferences.gamePixelPerBlock /= 2;
                            break;
                        case sf::Keyboard::K:
                            if (openMenuType == MENU_ENTERNAME) break;
                            scalingChanged = true;
                            preferences.uiScaling += 1;
                            break;
                        case sf::Keyboard::L:
                            if (openMenuType == MENU_ENTERNAME) break;
                            scalingChanged = true;
                            preferences.uiScaling -= 1;
                            break;
                        case sf::Keyboard::F3:
                            displayDebug ^= 1;
                            break;
                        case sf::Keyboard::F11:
                            preferences.fullScreenEnabled ^= 1;
                            if (preferences.fullScreenEnabled) {
                                sf::VideoMode fullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
                                window.create(fullScreenVideoMode, "Minecraft SFML Edition", sf::Style::Default | sf::Style::Fullscreen, ctxSettings);
                                resized = true;
                                screenRect = sf::FloatRect(0, 0, fullScreenVideoMode.width, fullScreenVideoMode.height);
                                window.setView(sf::View(screenRect));
                            } else {
                                screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
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
                case sf::Event::TextEntered:
                    textInputed += event.text.unicode;
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
                    preferences.screenSize = sf::Vector2i(event.size.width, event.size.height);
                    screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
                    window.setView(sf::View(screenRect));
                    break;
                default:
                    break;
            }
        }

        perfDebugInfo.endEventLoop();

        switch (openMenuType) {
            case MENU_NONE:
                if (heldInventory.getItemStack(0).id) {
                    mc::ItemStack droppedItemStack = heldInventory.getItemStack(0);
                    heldInventory.setItemStack(0, mc::ItemStack(0, 0));
                    droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                    if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                }
                for (int i = 0; i < 4; i++) {
                    if (crafting2x2_inventory.getInputItemStack(i).id) {
                        mc::ItemStack droppedItemStack = crafting2x2_inventory.getInputItemStack(i);
                        crafting2x2_inventory.setInputItemStack(i, mc::ItemStack(0, 0));
                        droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                        if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                    }
                }
                for (int i = 0; i < 9; i++) {
                    if (crafting3x3_table.getInputItemStack(i).id) {
                        mc::ItemStack droppedItemStack = crafting3x3_table.getInputItemStack(i);
                        crafting3x3_table.setInputItemStack(i, mc::ItemStack(0, 0));
                        droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                        if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                    }
                }
                if (unsavedChestEdit) {
                    std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}.dat.gz", worldName, openedChestChunkID, openedChestPos.x, openedChestPos.y);
                    chestInventory.saveToFile(filePath);
                    chestInventory.clear();
                    soundEffect.play("chest.close", 0.8f, mc::getLocationDelta(chunks.getPlayerChunkID(), chunks.getPlayerPos(), openedChestChunkID, sf::Vector2f(openedChestPos)));
                    unsavedChestEdit = false;
                }
                if (rightClick && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 39) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        openMenuType = MENU_CRAFTINGTABLE;
                    }
                } else if (rightClick && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 40) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}.dat.gz", worldName, chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y);
                        if (std::filesystem::exists(filePath)) {
                            chestInventory.loadFromFile(filePath);
                        } else {
                            chestInventory.clear();
                        }
                        openedChestChunkID = chunks.getMouseChunkID();
                        openedChestPos = chunks.getMousePos();
                        unsavedChestEdit = true;
                        soundEffect.play("chest.open", 0.8f, mc::getLocationDelta(chunks.getPlayerChunkID(), chunks.getPlayerPos(), openedChestChunkID, sf::Vector2f(openedChestPos)));
                        openMenuType = MENU_CHEST;
                    }
                } else if (rightClick && (chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 41 || chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 42)) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        openedFurnaceChunkID = chunks.getMouseChunkID();
                        openedFurnacePos = chunks.getMousePos();
                        openMenuType = MENU_FURNACE;
                    }
                } else if (rightClickHeld && tickCount - lastPlaceTickCount >= 4 && (chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 2 || chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 3) && hotbarInventory.getItemStack(selectedHotbarSlot).id >= 82 && hotbarInventory.getItemStack(selectedHotbarSlot).id < 87 && (chunks.getMousePos().y == 0 || chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y - 1) == 0)) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        soundEffect.play("hoe.till", 1.f, mc::getLocationDelta(chunks.getPlayerChunkID(), chunks.getPlayerPos(), chunks.getMouseChunkID(), sf::Vector2f(chunks.getMousePos())));
                        chunks.setBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y, 51);
                    }
                } else if (rightClickHeld && tickCount - lastPlaceTickCount >= 4 && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 11 && hotbarInventory.getItemStack(selectedHotbarSlot).id == 108) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        lastPlaceTickCount = tickCount;
                        soundEffect.play("bucket.fill", 1.f, mc::getLocationDelta(chunks.getPlayerChunkID(), chunks.getPlayerPos(), chunks.getMouseChunkID(), sf::Vector2f(chunks.getMousePos())));
                        chunks.setBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y, 0);
                        hotbarInventory.subtractItem(selectedHotbarSlot, 1);
                        mc::ItemStack droppedItemStack(109, 1);
                        droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                        if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                    }
                } else if (rightClickHeld && tickCount - lastPlaceTickCount >= 4 && chunks.getBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y) == 13 && hotbarInventory.getItemStack(selectedHotbarSlot).id == 108) {
                    sf::Vector2f playerCenter(std::floor(chunks.getPlayerPos().x), std::ceil(chunks.getPlayerPos().y) - 1.5f);
                    sf::Vector2f mousePosF(chunks.getMousePos());
                    sf::Vector2f dist = mousePosF - playerCenter;
                    dist.x += static_cast<float>(chunks.getMouseChunkID() - chunks.getPlayerChunkID()) * 16.f;
                    if (dist.y >= 0) {
                        dist.y -= 0.5f;
                        std::max(dist.y, 0.f);
                    } else {
                        dist.y += 0.5f;
                        std::min(dist.y, 0.f);
                    }
                    if (sqrtf(dist.x * dist.x + dist.y * dist.y) <= 3.f) {
                        lastPlaceTickCount = tickCount;
                        soundEffect.play("bucket.fill_lava", 1.f, mc::getLocationDelta(chunks.getPlayerChunkID(), chunks.getPlayerPos(), chunks.getMouseChunkID(), sf::Vector2f(chunks.getMousePos())));
                        chunks.setBlock(chunks.getMouseChunkID(), chunks.getMousePos().x, chunks.getMousePos().y, 0);
                        hotbarInventory.subtractItem(selectedHotbarSlot, 1);
                        mc::ItemStack droppedItemStack(110, 1);
                        droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                        if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                    }
                }
                break;
            case MENU_PLAYERINV:
                // Hotbar
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = hotbarInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick && (heldInventory.getItemStack(0).id == 0 || (heldInventory.getItemStack(0).id == crafting3x3_table.getOutputItemStack(0).id && heldInventory.getEmptySpace(0) >= crafting3x3_table.getOutputItemStack(0).amount))) {
                            heldInventory.addItemStack(0, crafting3x3_table.takeOutputItem(0));
                        }
                    }
                }
                break;
            case MENU_FURNACE:
                furnaceInterface.setFurnaceData(chunks.getChunk(openedFurnaceChunkID).getFurnaceData(openedFurnacePos.x, openedFurnacePos.y));
                // Hotbar
                for (int i = 0; i < 9; i++) {
                    sf::FloatRect bound = hotbarInventory.getSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
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
                // Furnace input
                for (int i = 0; i < 1; i++) {
                    sf::FloatRect bound = furnaceInterface.getInputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (furnaceInterface.getInputItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, furnaceInterface.addInputItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(furnaceInterface.getInputItemStack(i));
                                furnaceInterface.setInputItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(furnaceInterface.getInputItemStack(i).id, furnaceInterface.getInputItemStack(i).amount / 2 + furnaceInterface.getInputItemStack(i).amount % 2);
                                furnaceInterface.subtractInputItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (furnaceInterface.getInputEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (furnaceInterface.getInputItemStack(i).id == heldInventory.getItemStack(0).id || furnaceInterface.getInputItemStack(i).id == 0)) {
                                furnaceInterface.addInputItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Furnace fuel
                for (int i = 0; i < 1; i++) {
                    sf::FloatRect bound = furnaceInterface.getFuelSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick) {
                            if (furnaceInterface.getFuelItemStack(i).id == heldInventory.getItemStack(0).id) {
                                heldInventory.setItemStack(0, furnaceInterface.addFuelItemStack(i, heldInventory.getItemStack(0)));
                            } else {
                                mc::ItemStack tempItemStack(furnaceInterface.getFuelItemStack(i));
                                furnaceInterface.setFuelItemStack(i, heldInventory.getItemStack(0));
                                heldInventory.setItemStack(0, tempItemStack);
                            }
                        } else if (rightClick) {
                            if (heldInventory.getItemStack(0).id == 0) {
                                mc::ItemStack movedItemStack(furnaceInterface.getFuelItemStack(i).id, furnaceInterface.getFuelItemStack(i).amount / 2 + furnaceInterface.getFuelItemStack(i).amount % 2);
                                furnaceInterface.subtractFuelItem(i, movedItemStack.amount);
                                heldInventory.setItemStack(0, movedItemStack);
                            } else if (furnaceInterface.getFuelEmptySpace(i) > 0 && heldInventory.getItemStack(0).id != 0 && (furnaceInterface.getFuelItemStack(i).id == heldInventory.getItemStack(0).id || furnaceInterface.getFuelItemStack(i).id == 0)) {
                                furnaceInterface.addFuelItemStack(i, mc::ItemStack(heldInventory.getItemStack(0).id, 1));
                                heldInventory.subtractItem(0, 1);
                            }
                        }
                    }
                }
                // Furnace output
                for (int i = 0; i < 1; i++) {
                    sf::FloatRect bound = furnaceInterface.getOutputSlotGlobalBounds(i);
                    if (bound.contains(sf::Vector2f(mousePosition))) {
                        inventorySlotHoverHighlighter.setPosition(bound.getPosition() + sf::Vector2f(1.f * preferences.uiScaling, 1.f * preferences.uiScaling));
                        inventorySlotHoverHighlighter.setSize(bound.getSize() - sf::Vector2f(2.f * preferences.uiScaling, 2.f * preferences.uiScaling));
                        renderSlotHoverHighlighter = true;
                        if (leftClick && (heldInventory.getItemStack(0).id == 0 || (heldInventory.getItemStack(0).id == furnaceInterface.getOutputItemStack(0).id && heldInventory.getEmptySpace(0) >= furnaceInterface.getOutputItemStack(0).amount))) {
                            heldInventory.addItemStack(0, furnaceInterface.takeOutputItem(0));
                        }
                    }
                }
                chunks.getChunk(openedFurnaceChunkID).setFurnaceData(openedFurnacePos.x, openedFurnacePos.y, furnaceInterface.getFurnaceData());
                break;
            case MENU_SETTINGS:
                if (masterVolumeSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    masterVolumeSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        masterVolumeSlider.setActiveValue(true);
                        soundEffect.play("click");
                        volumeChanged = true;
                        masterVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.masterVolume = masterVolumeSlider.getValue();
                        masterVolumeSlider.setDisplayText(std::format("Volume: {}%", preferences.masterVolume));
                    } else if (leftClickHeld && masterVolumeSlider.getActiveValue()) {
                        volumeChanged = true;
                        masterVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.masterVolume = masterVolumeSlider.getValue();
                        masterVolumeSlider.setDisplayText(std::format("Volume: {}%", preferences.masterVolume));
                    }
                } else {
                    masterVolumeSlider.setActiveValue(false);
                    masterVolumeSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (musicVolumeSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    musicVolumeSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        musicVolumeSlider.setActiveValue(true);
                        soundEffect.play("click");
                        volumeChanged = true;
                        musicVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.musicVolume = musicVolumeSlider.getValue();
                        musicVolumeSlider.setDisplayText(std::format("Music: {}%", preferences.musicVolume));
                    } else if (leftClickHeld && musicVolumeSlider.getActiveValue()) {
                        volumeChanged = true;
                        musicVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.musicVolume = musicVolumeSlider.getValue();
                        musicVolumeSlider.setDisplayText(std::format("Music: {}%", preferences.musicVolume));
                    }
                } else {
                    musicVolumeSlider.setActiveValue(false);
                    musicVolumeSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (antialiasingLevelSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    antialiasingLevelSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowNeedRecreate = true;
                        antialiasingLevelSlider.setValueByMousePos(mousePosition);
                        preferences.antialiasingLevel = antialiasingLevelSlider.getValue();
                        antialiasingLevelSlider.setDisplayText(std::format("Antialiasing: {}", preferences.antialiasingLevel));
                    }
                } else {
                    antialiasingLevelSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (framerateLimitSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    framerateLimitSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        framerateLimitSlider.setActiveValue(true);
                        soundEffect.play("click");
                        windowSettingsChanged = true;
                        framerateLimitSlider.setValueByMousePos(mousePosition);
                        preferences.framerateLimit = framerateLimitSlider.getValue() * 10;
                        if (preferences.framerateLimit != 0) {
                            framerateLimitSlider.setDisplayText(std::format("Max Framerate: {} fps", preferences.framerateLimit));
                        } else {
                            framerateLimitSlider.setDisplayText("Max Framerate: Unlimited");
                        }
                    } else if (leftClickHeld && framerateLimitSlider.getActiveValue()) {
                        windowSettingsChanged = true;
                        framerateLimitSlider.setValueByMousePos(mousePosition);
                        preferences.framerateLimit = framerateLimitSlider.getValue() * 10;
                        if (preferences.framerateLimit != 0) {
                            framerateLimitSlider.setDisplayText(std::format("Max Framerate: {} fps", preferences.framerateLimit));
                        } else {
                            framerateLimitSlider.setDisplayText("Max Framerate: Unlimited");
                        }
                    }
                } else {
                    framerateLimitSlider.setActiveValue(false);
                    framerateLimitSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (ppbSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    ppbSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        ppbSlider.setActiveValue(true);
                        soundEffect.play("click");
                        ppbChanged = true;
                        ppbSlider.setValueByMousePos(mousePosition);
                        preferences.gamePixelPerBlock = 0b1 << ppbSlider.getValue();
                        ppbSlider.setDisplayText(std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock));
                    } else if (leftClickHeld && ppbSlider.getActiveValue()) {
                        ppbChanged = true;
                        ppbSlider.setValueByMousePos(mousePosition);
                        preferences.gamePixelPerBlock = 0b1 << ppbSlider.getValue();
                        ppbSlider.setDisplayText(std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock));
                    }
                } else {
                    ppbSlider.setActiveValue(false);
                    ppbSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (fullscreenToggleButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    fullscreenToggleButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowNeedRecreate = true;
                        preferences.fullScreenEnabled ^= 1;
                        fullscreenToggleButton.setDisplayText(preferences.fullScreenEnabled ? "Fullscreen: ON" : "Fullscreen: OFF");
                    }
                } else {
                    fullscreenToggleButton.setState(BTN_STATE_NORMAL);
                }
                if (vsyncToggleButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    vsyncToggleButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowSettingsChanged = true;
                        preferences.vsyncEnabled ^= 1;
                        vsyncToggleButton.setDisplayText(preferences.vsyncEnabled ? "VSync: ON" : "VSync: OFF");
                    }
                } else {
                    vsyncToggleButton.setState(BTN_STATE_NORMAL);
                }
                if (uiScalingStepButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    uiScalingStepButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        scalingChanged = true;
                        preferences.uiScaling++;
                        preferences.uiScaling %= 5;
                        preferences.uiScaling = std::clamp(preferences.uiScaling, 1, 4);
                        uiScalingStepButton.setDisplayText(std::format("GUI Scale: {}", preferences.uiScaling));
                    }
                } else {
                    uiScalingStepButton.setState(BTN_STATE_NORMAL);
                }
                if (settingsDoneButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    settingsDoneButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_PAUSE;
                    }
                } else {
                    settingsDoneButton.setState(BTN_STATE_NORMAL);
                }
                break;
            case MENU_PAUSE:
                if (backToGameButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    backToGameButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_NONE;
                    }
                } else {
                    backToGameButton.setState(BTN_STATE_NORMAL);
                }
                if (optionsButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    optionsButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_SETTINGS;
                    }
                } else {
                    optionsButton.setState(BTN_STATE_NORMAL);
                }
                if (saveAndQuitButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    saveAndQuitButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        if (unsavedChestEdit) {
                            std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}.dat.gz", worldName, openedChestChunkID, openedChestPos.x, openedChestPos.y);
                            chestInventory.saveToFile(filePath);
                        }
                        chunks.saveAll();
                        std::ofstream seedFile(std::format("saves/{}/seed.dat", worldName), std::ios::binary);
                        seedFile.write(reinterpret_cast<char*>(&seed), sizeof(int));
                        seedFile.close();
                        playtime += tickCount;
                        std::ofstream playtimeFile(std::format("saves/{}/playtime.dat", worldName), std::ios::binary);
                        playtimeFile.write(reinterpret_cast<char*>(&playtime), sizeof(int));
                        playtimeFile.close();
                        hotbarInventory.saveToFile(std::format("saves/{}/inventories/player/hotbar.dat.gz", worldName));
                        mainInventory.saveToFile(std::format("saves/{}/inventories/player/main.dat.gz", worldName));
                        mc::Player::saveDataToFile(std::format("saves/{}/player.dat.gz", worldName), mc::PlayerLocationData(chunks.getPlayerChunkID(), chunks.getPlayerPos()));
                        return;
                    }
                } else {
                    saveAndQuitButton.setState(BTN_STATE_NORMAL);
                }
                break;
            case MENU_ENTERNAME:
                if (leftClick) {
                    if (playerNameTextBox.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                        playerNameTextBox.setFocused(true);
                    } else {
                        playerNameTextBox.setFocused(false);
                    }
                }
                if (playerNameTextBox.getFocused()) {
                    playerNameTextBox.setDisplayString(playerNameTextBox.getDisplayString() + textInputed);
                }
                if (enterNameDoneButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    enterNameDoneButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        std::ifstream leaderboardFile("leaderboard.json");
                        json leaderboardData = json::parse(leaderboardFile);
                        leaderboardFile.close();
                        playtime += tickCount;
                        tickCount = 0;
                        sf::String playerName = playerNameTextBox.getDisplayString();
                        std::vector<uint32_t> playerNameUtf32;
                        for (sf::Uint32 c : playerName.toUtf32()) {
                            playerNameUtf32.push_back(c);
                        }
                        leaderboardData.push_back(json({playtime, json(playerNameUtf32)}));
                        std::ofstream leaderBoardFile("leaderboard.json");
                        leaderBoardFile << std::setw(4) << leaderboardData << std::endl;
                        leaderBoardFile.close();
                        openMenuType = MENU_NONE;
                    }
                } else {
                    enterNameDoneButton.setState(BTN_STATE_NORMAL);
                }
                break;
            default:
                break;
        }

        playerMoveInput = std::clamp(playerMoveInput, -1, 1);
        preferences.gamePixelPerBlock = std::clamp(preferences.gamePixelPerBlock, 1, 256);
        preferences.uiScaling = std::clamp(preferences.uiScaling, 1, 4);

        if (resized) {
            chunks.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
            player.setScreenSize(sf::Vector2i(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))));
            gameDebugInfo.setPosition(sf::Vector2f(screenRect.width - 5.f, 0.f));
        }

        if (ppbChanged) {
            chunks.setPixelPerBlock(preferences.gamePixelPerBlock);
            player.setPixelPerBlock(preferences.gamePixelPerBlock);
        }

        if (scalingChanged) {
            hotbarInventory.setScaling(preferences.uiScaling);
            mainInventory.setScaling(preferences.uiScaling);
            mainInventorySprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
            chestInventorySprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
            craftingTableInventorySprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
            furnaceInventorySprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
            crafting2x2_inventory.setScaling(preferences.uiScaling);
            crafting3x3_table.setScaling(preferences.uiScaling);
            furnaceInterface.setScaling(preferences.uiScaling);
            heldInventory.setScaling(preferences.uiScaling);
            heldInventory.setOrigin(sf::Vector2f(heldInventory.getSlotLocalBounds(0).width / 2.f, heldInventory.getSlotLocalBounds(0).height / 2.f));
        }

        if (playerIntendJump) {
            player.jump();
        }
        
        player.setLateralForce(playerMoveInput, playerSprinting);

        if (openMenuType == MENU_NONE || openMenuType == MENU_PLAYERINV || openMenuType == MENU_CHEST || openMenuType == MENU_CRAFTINGTABLE || openMenuType == MENU_FURNACE) {
            player.update(frameTime);
        }

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

        if (windowSettingsChanged) {
            window.setFramerateLimit(preferences.framerateLimit);
            window.setVerticalSyncEnabled(preferences.vsyncEnabled);
        }

        if (windowNeedRecreate) {
            ctxSettings.antialiasingLevel = preferences.antialiasingLevel;
            if (preferences.fullScreenEnabled) {
                sf::VideoMode fullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
                window.create(fullScreenVideoMode, "Minecraft SFML Edition", sf::Style::Default | sf::Style::Fullscreen, ctxSettings);
                screenRect = sf::FloatRect(0, 0, fullScreenVideoMode.width, fullScreenVideoMode.height);
                window.setView(sf::View(screenRect));
            } else {
                screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
                window.create(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "Minecraft SFML Edition", sf::Style::Default, ctxSettings);
                window.setView(sf::View(screenRect));
            }
            window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
            window.setFramerateLimit(preferences.framerateLimit);
            window.setVerticalSyncEnabled(preferences.vsyncEnabled);
            window.setKeyRepeatEnabled(false);
        }

        if (volumeChanged) {
            sf::Listener::setGlobalVolume(static_cast<float>(preferences.masterVolume));
            musicPlayer.setVolume(static_cast<float>(preferences.musicVolume) / 100.f);
        }

        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        if (elapsedTime.asMilliseconds() - lastTickTimeMs >= 50 && (openMenuType == MENU_NONE || openMenuType == MENU_PLAYERINV || openMenuType == MENU_CHEST || openMenuType == MENU_CRAFTINGTABLE || openMenuType == MENU_FURNACE)) {
            lastTickTimeMs += 50 + std::max(idiv(elapsedTime.asMilliseconds() - lastTickTimeMs, 50) - 100, 0) * 50;
            if (leftClickHeld && openMenuType == MENU_NONE) {
                std::vector<mc::ItemStack> droppedItemStacks = chunks.breakBlock(hotbarInventory.getItemStack(selectedHotbarSlot).id, tickCount);
                for (mc::ItemStack& droppedItemStack : droppedItemStacks) {
                    if (droppedItemStack.id == 53 && !diamondAcquired) {
                        std::ofstream flagFile(std::format("saves/{}/diamondAcquired", worldName), std::ios::binary);
                        flagFile.close();
                        diamondAcquired = true;
                        openMenuType = MENU_ENTERNAME;
                        playerNameTextBox.setFocused(true);
                    }
                    if (droppedItemStack.id != 0) {
                        droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                        if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                    }
                }
            }
            if (rightClickHeld && openMenuType == MENU_NONE && tickCount - lastPlaceTickCount >= 4) {
                int itemID = hotbarInventory.getItemStack(selectedHotbarSlot).id;
                if (chunks.placeBlock(itemID)) {
                    lastPlaceTickCount = tickCount;
                    if (itemID != 81) {
                        hotbarInventory.subtractItem(selectedHotbarSlot, 1);
                        if (itemID == 109 || itemID == 110) {
                            mc::ItemStack droppedItemStack(108, 1);
                            droppedItemStack = hotbarInventory.addItemStack(droppedItemStack);
                            if (droppedItemStack.amount > 0) droppedItemStack = mainInventory.addItemStack(droppedItemStack);
                        }
                    }
                }
            }

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

        switch (openMenuType) {
            case MENU_NONE:
                hotbarInventory.setMargin(2);
                hotbarInventory.setPosition(sf::Vector2f(screenRect.width / 2.f - hotbarInventory.getLocalBounds().width / 2.f, screenRect.height - hotbarInventory.getLocalBounds().height));
                hotbarInventorySprite.setPosition(sf::Vector2f(hotbarInventory.getGlobalBounds().left - static_cast<float>(preferences.uiScaling), hotbarInventory.getGlobalBounds().top - static_cast<float>(preferences.uiScaling)));
                hotbarInventorySprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
                selectedHotbarSlotSprite.setScale(sf::Vector2f(preferences.uiScaling, preferences.uiScaling));
                window.setMouseCursor(crossCursor);
                break;
            case MENU_PLAYERINV:
                mainInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                mainInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), mainInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(preferences.uiScaling)));
                hotbarInventory.setMargin(1);
                hotbarInventory.setPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), mainInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(preferences.uiScaling)));
                crafting2x2_inventory.setInputPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 88.f * static_cast<float>(preferences.uiScaling), mainInventorySprite.getGlobalBounds().top + 25.f * static_cast<float>(preferences.uiScaling)));
                crafting2x2_inventory.setOutputPosition(sf::Vector2f(mainInventorySprite.getGlobalBounds().left + 144.f * static_cast<float>(preferences.uiScaling), mainInventorySprite.getGlobalBounds().top + 35.f * static_cast<float>(preferences.uiScaling)));
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_PAUSE:
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                gameMenuText.setCharacterSize(12 * preferences.uiScaling);
                gameMenuText.setOutlineThickness(0.5f * preferences.uiScaling);
                backToGameButton.setScaling(preferences.uiScaling);
                optionsButton.setScaling(preferences.uiScaling);
                saveAndQuitButton.setScaling(preferences.uiScaling);

                gameMenuText.setPosition(sf::Vector2f(screenRect.width / 2.f - gameMenuText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                backToGameButton.setPosition(sf::Vector2f(screenRect.width / 2.f - backToGameButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f));
                optionsButton.setPosition(sf::Vector2f(screenRect.width / 2.f - optionsButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f + static_cast<float>(preferences.uiScaling * 25)));
                saveAndQuitButton.setPosition(sf::Vector2f(screenRect.width / 2.f - saveAndQuitButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_SETTINGS:
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                optionText.setCharacterSize(12 * preferences.uiScaling);
                optionText.setOutlineThickness(0.5f * preferences.uiScaling);
                masterVolumeSlider.setScaling(preferences.uiScaling);
                musicVolumeSlider.setScaling(preferences.uiScaling);
                antialiasingLevelSlider.setScaling(preferences.uiScaling);
                framerateLimitSlider.setScaling(preferences.uiScaling);
                fullscreenToggleButton.setScaling(preferences.uiScaling);
                vsyncToggleButton.setScaling(preferences.uiScaling);
                uiScalingStepButton.setScaling(preferences.uiScaling);
                ppbSlider.setScaling(preferences.uiScaling);
                settingsDoneButton.setScaling(preferences.uiScaling);

                optionText.setPosition(sf::Vector2f(screenRect.width / 2.f - optionText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                masterVolumeSlider.setPosition(sf::Vector2f(screenRect.width / 2.f - masterVolumeSlider.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f));
                musicVolumeSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f));
                antialiasingLevelSlider.setPosition(sf::Vector2f(screenRect.width / 2.f - antialiasingLevelSlider.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25)));
                framerateLimitSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25)));
                fullscreenToggleButton.setPosition(sf::Vector2f(screenRect.width / 2.f - fullscreenToggleButton.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                vsyncToggleButton.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                uiScalingStepButton.setPosition(sf::Vector2f(screenRect.width / 2.f - uiScalingStepButton.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 3.f));
                ppbSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 3.f));
                settingsDoneButton.setPosition(sf::Vector2f(screenRect.width / 2.f - settingsDoneButton.getGlobalBounds().width / 2.f, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 4.5f));

                masterVolumeSlider.setValue(preferences.masterVolume);
                musicVolumeSlider.setValue(preferences.musicVolume);
                antialiasingLevelSlider.setValue(preferences.antialiasingLevel);
                framerateLimitSlider.setValue(preferences.framerateLimit / 10);
                ppbSlider.setValue(static_cast<int>(std::round(log2(preferences.gamePixelPerBlock))));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_CRAFTINGTABLE:
                craftingTableInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                mainInventory.setPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(preferences.uiScaling)));
                hotbarInventory.setMargin(1);
                hotbarInventory.setPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(preferences.uiScaling)));
                crafting3x3_table.setInputPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 29.f * static_cast<float>(preferences.uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 16.f * static_cast<float>(preferences.uiScaling)));
                crafting3x3_table.setOutputPosition(sf::Vector2f(craftingTableInventorySprite.getGlobalBounds().left + 123.f * static_cast<float>(preferences.uiScaling), craftingTableInventorySprite.getGlobalBounds().top + 34.f * static_cast<float>(preferences.uiScaling)));
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_CHEST:
                chestInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                mainInventory.setPosition(sf::Vector2f(chestInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), chestInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(preferences.uiScaling)));
                hotbarInventory.setMargin(1);
                hotbarInventory.setPosition(sf::Vector2f(chestInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), chestInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(preferences.uiScaling)));
                chestInventory.setPosition(sf::Vector2f(chestInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), chestInventorySprite.getGlobalBounds().top + 17.f * static_cast<float>(preferences.uiScaling)));
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_FURNACE:
                furnaceInventorySprite.setPosition(sf::Vector2f(screenRect.width / 2.f, screenRect.height / 2.f));
                mainInventory.setPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 83.f * static_cast<float>(preferences.uiScaling)));
                hotbarInventory.setMargin(1);
                hotbarInventory.setPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 7.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 141.f * static_cast<float>(preferences.uiScaling)));
                furnaceInterface.setInputPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 55.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 16.f * static_cast<float>(preferences.uiScaling)));
                furnaceInterface.setFuelPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 55.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 52.f * static_cast<float>(preferences.uiScaling)));
                furnaceInterface.setOutputPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 115.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 34.f * static_cast<float>(preferences.uiScaling)));
                furnaceInterface.setProgressBarPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 79.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 34.f * static_cast<float>(preferences.uiScaling)));
                furnaceInterface.setFuelBarPosition(sf::Vector2f(furnaceInventorySprite.getGlobalBounds().left + 56.f * static_cast<float>(preferences.uiScaling), furnaceInventorySprite.getGlobalBounds().top + 36.f * static_cast<float>(preferences.uiScaling)));
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                window.setMouseCursor(arrowCursor);
                break;
            case MENU_ENTERNAME:
                menuBlackOutBackground.setSize(sf::Vector2f(screenRect.width, screenRect.height));
                enterNameText.setCharacterSize(12 * preferences.uiScaling);
                enterNameText.setOutlineThickness(0.5f * preferences.uiScaling);
                playerNameTextBox.setScaling(preferences.uiScaling);
                enterNameDoneButton.setScaling(preferences.uiScaling);

                enterNameText.setPosition(sf::Vector2f(screenRect.width / 2.f - enterNameText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                playerNameTextBox.setPosition(sf::Vector2f(screenRect.width / 2.f - playerNameTextBox.getGlobalBounds().width / 2.f, screenRect.height * 0.35f));
                enterNameDoneButton.setPosition(sf::Vector2f(screenRect.width / 2.f - enterNameDoneButton.getGlobalBounds().width / 2.f, screenRect.height * 0.35f + static_cast<float>(preferences.uiScaling * 25) * 1.5f));
                window.setMouseCursor(arrowCursor);
                break;
            default:
                break;
        }

        selectedHotbarSlotSprite.setPosition(sf::Vector2f(hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).left - static_cast<float>(preferences.uiScaling) * 2.f, hotbarInventory.getSlotGlobalBounds(selectedHotbarSlot).top - static_cast<float>(preferences.uiScaling) * 2.f));

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
                window.draw(menuBlackOutBackground);
                window.draw(gameMenuText);
                window.draw(backToGameButton);
                window.draw(optionsButton);
                window.draw(saveAndQuitButton);
                break;
            case MENU_SETTINGS:
                window.draw(menuBlackOutBackground);
                window.draw(optionText);
                window.draw(masterVolumeSlider);
                window.draw(musicVolumeSlider);
                window.draw(antialiasingLevelSlider);
                window.draw(framerateLimitSlider);
                window.draw(fullscreenToggleButton);
                window.draw(vsyncToggleButton);
                window.draw(uiScalingStepButton);
                window.draw(ppbSlider);
                window.draw(settingsDoneButton);
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
                heldInventory.setPosition(sf::Vector2f(mousePosition));
                window.draw(menuBlackOutBackground);
                window.draw(furnaceInventorySprite);
                window.draw(mainInventory);
                window.draw(hotbarInventory);
                window.draw(furnaceInterface);
                if (renderSlotHoverHighlighter) window.draw(inventorySlotHoverHighlighter);
                window.draw(heldInventory);
                break;
            case MENU_ENTERNAME:
                window.draw(menuBlackOutBackground);
                window.draw(enterNameText);
                window.draw(playerNameTextBox);
                window.draw(enterNameDoneButton);
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
}

int main() {
    srand(time(NULL));

    mc::PreferencesData preferences;
    if (std::filesystem::exists("settings.dat.gz")) {
        preferences = mc::Preferences::loadFromFile("settings.dat.gz");
    } else {
        preferences = mc::Preferences::getDefault();
    }

    sf::ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = preferences.antialiasingLevel;

    sf::Cursor arrowCursor;
    arrowCursor.loadFromSystem(sf::Cursor::Arrow);

    sf::FloatRect screenRect;
    sf::RenderWindow window;

    if (preferences.fullScreenEnabled) {
        sf::VideoMode fullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
        window.create(fullScreenVideoMode, "Minecraft SFML Edition", sf::Style::Default | sf::Style::Fullscreen, ctxSettings);
        screenRect = sf::FloatRect(0, 0, fullScreenVideoMode.width, fullScreenVideoMode.height);
        window.setView(sf::View(screenRect));
    } else {
        screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
        window.create(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "Minecraft SFML Edition", sf::Style::Default, ctxSettings);
        window.setView(sf::View(screenRect));
    }

    window.setFramerateLimit(preferences.framerateLimit);
    window.setVerticalSyncEnabled(preferences.vsyncEnabled);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursor(arrowCursor);

    sf::Image icon;
    icon.loadFromFile("resources/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    if (!std::filesystem::exists("leaderboard.json")) {
        std::ofstream leaderboardJsonFile("leaderboard.json");
        leaderboardJsonFile << "[]";
        leaderboardJsonFile.close();
    }

    sf::Listener::setPosition(sf::Vector3f(0.f, 0.f, 0.f));
    sf::Listener::setDirection(sf::Vector3f(0.f, 0.f, -1.f));
    sf::Listener::setUpVector(sf::Vector3f(0.f, 1.f, 0.f));
    sf::Listener::setGlobalVolume(static_cast<float>(preferences.masterVolume));

    std::vector<std::string> musicNames = {"menu1.ogg", "menu2.ogg", "menu3.ogg", "menu4.ogg"};
    mc::MusicPlayer musicPlayer("resources/sounds/music/menu/", musicNames, static_cast<float>(preferences.musicVolume) / 100.f);
    musicPlayer.start();

    mc::SoundEffect soundEffect("resources/sounds/event/");

    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    sf::Font robotoMonoRegular;
    robotoMonoRegular.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");

    sf::Font sarabunRegular;
    sarabunRegular.loadFromFile("resources/fonts/Sarabun-Regular.ttf");

    sf::Texture shortButtonTexture;
    shortButtonTexture.loadFromFile("resources/textures/gui/buttonShort.png");
    sf::Texture mediumButtonTexture;
    mediumButtonTexture.loadFromFile("resources/textures/gui/buttonMedium.png");
    sf::Texture longButtonTexture;
    longButtonTexture.loadFromFile("resources/textures/gui/buttonLong.png");

    sf::Texture shortSliderTexture;
    shortSliderTexture.loadFromFile("resources/textures/gui/sliderShort.png");
    sf::Texture mediumSliderTexture;
    mediumSliderTexture.loadFromFile("resources/textures/gui/sliderMedium.png");
    sf::Texture longSliderTexture;
    longSliderTexture.loadFromFile("resources/textures/gui/sliderLong.png");
    sf::Texture slidingTexture;
    slidingTexture.loadFromFile("resources/textures/gui/sliderSliding.png");

    mc::Button singleplayerButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Singleplayer");
    mc::Button leaderBoardButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Leaderboard");
    mc::Button optionsButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Options...");
    mc::Button quitGameButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Quit Game");

    sf::Text versionText;
    versionText.setFont(robotoRegular);
    versionText.setLetterSpacing(1.25f);
    versionText.setCharacterSize(12 * preferences.uiScaling);
    versionText.setFillColor(sf::Color::White);
    versionText.setOutlineColor(sf::Color::Black);
    versionText.setOutlineThickness(0.5f * preferences.uiScaling);
    versionText.setString("Minecraft SFML Edition 1.0.0");

    sf::Text copyleftText;
    copyleftText.setFont(robotoRegular);
    copyleftText.setLetterSpacing(1.25f);
    copyleftText.setCharacterSize(12 * preferences.uiScaling);
    copyleftText.setFillColor(sf::Color::White);
    copyleftText.setOutlineColor(sf::Color::Black);
    copyleftText.setOutlineThickness(0.5f * preferences.uiScaling);
    copyleftText.setString("Copyleft 66010261 Natthapas Saengnikornkiat");

    sf::Text technobladeNeverDiesText;
    technobladeNeverDiesText.setFont(robotoRegular);
    technobladeNeverDiesText.setLetterSpacing(1.25f);
    technobladeNeverDiesText.setCharacterSize(11 * preferences.uiScaling);
    technobladeNeverDiesText.setFillColor(sf::Color(252, 252, 0));
    technobladeNeverDiesText.setOutlineColor(sf::Color(62, 62, 0));
    technobladeNeverDiesText.setOutlineThickness(0.5f * preferences.uiScaling);
    technobladeNeverDiesText.setString("Technoblade never dies!");
    technobladeNeverDiesText.setOrigin(sf::Vector2f(technobladeNeverDiesText.getGlobalBounds().width / 2.f, technobladeNeverDiesText.getGlobalBounds().height / 2.f));
    technobladeNeverDiesText.setRotation(-20.f);

    sf::Texture titleTexture;
    titleTexture.loadFromFile("resources/textures/gui/title.png");
    titleTexture.setSmooth(true);
    sf::Sprite titleSprite(titleTexture);

    sf::Texture menuBackgroundTexture;
    menuBackgroundTexture.loadFromFile("resources/textures/gui/menuBackground.png");
    menuBackgroundTexture.setSmooth(true);
    mc::MenuBackground menuBackground(menuBackgroundTexture);

    sf::Texture dirtBackgroundTexture;
    dirtBackgroundTexture.loadFromFile("resources/textures/gui/dirtBackground.png");
    dirtBackgroundTexture.setRepeated(true);
    sf::Sprite dirtBackground(dirtBackgroundTexture);

    sf::Text optionText;
    optionText.setFont(robotoRegular);
    optionText.setLetterSpacing(1.25f);
    optionText.setCharacterSize(12 * preferences.uiScaling);
    optionText.setFillColor(sf::Color::White);
    optionText.setOutlineColor(sf::Color::Black);
    optionText.setOutlineThickness(0.5f * preferences.uiScaling);
    optionText.setString("Options");
    mc::Slider masterVolumeSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Volume: {}%", preferences.masterVolume), 100);
    mc::Slider musicVolumeSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Music: {}%", preferences.musicVolume), 100);
    mc::Slider antialiasingLevelSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Antialiasing: {}", preferences.antialiasingLevel), 8);
    mc::Slider framerateLimitSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, preferences.framerateLimit != 0 ? std::format("Max Framerate: {} fps", preferences.framerateLimit) : "Max Framerate: Unlimited", 25);
    mc::Button fullscreenToggleButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, preferences.fullScreenEnabled ? "Fullscreen: ON" : "Fullscreen: OFF");
    mc::Button vsyncToggleButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, preferences.vsyncEnabled ? "VSync: ON" : "VSync: OFF");
    mc::Button uiScalingStepButton(mediumButtonTexture, robotoRegular, preferences.uiScaling, std::format("GUI Scale: {}", preferences.uiScaling));
    mc::Slider ppbSlider(mediumSliderTexture, slidingTexture, robotoRegular, preferences.uiScaling, std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock), 8);
    mc::Button settingsDoneButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Done");

    sf::Text selectWorldText;
    selectWorldText.setFont(robotoRegular);
    selectWorldText.setLetterSpacing(1.25f);
    selectWorldText.setCharacterSize(12 * preferences.uiScaling);
    selectWorldText.setFillColor(sf::Color::White);
    selectWorldText.setOutlineColor(sf::Color::Black);
    selectWorldText.setOutlineThickness(0.5f * preferences.uiScaling);
    selectWorldText.setString("Select World");
    mc::TextBox worldNameTextBox(sarabunRegular, sf::Vector2f(200.f, 20.f), preferences.uiScaling);
    mc::Button selectWorldDoneButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Done");

    sf::Text leaderboardText;
    leaderboardText.setFont(robotoRegular);
    leaderboardText.setLetterSpacing(1.25f);
    leaderboardText.setCharacterSize(12 * preferences.uiScaling);
    leaderboardText.setFillColor(sf::Color::White);
    leaderboardText.setOutlineColor(sf::Color::Black);
    leaderboardText.setOutlineThickness(0.5f * preferences.uiScaling);
    leaderboardText.setString("Leaderboard");
    std::vector<sf::Text> leaderboardPlayersName;
    std::vector<sf::Text> leaderboardPlayersTime;
    leaderboardPlayersName.resize(8);
    leaderboardPlayersTime.resize(8);
    for (size_t i = 0; i < leaderboardPlayersName.size(); i++) {
        leaderboardPlayersName[i].setFont(sarabunRegular);
        leaderboardPlayersName[i].setLetterSpacing(1.25f);
        leaderboardPlayersName[i].setCharacterSize(12 * preferences.uiScaling);
        leaderboardPlayersName[i].setFillColor(sf::Color::White);
        leaderboardPlayersName[i].setOutlineColor(sf::Color::Black);
        leaderboardPlayersName[i].setOutlineThickness(0.5f * preferences.uiScaling);
        leaderboardPlayersTime[i].setFont(sarabunRegular);
        leaderboardPlayersTime[i].setLetterSpacing(1.25f);
        leaderboardPlayersTime[i].setCharacterSize(12 * preferences.uiScaling);
        leaderboardPlayersTime[i].setFillColor(sf::Color::White);
        leaderboardPlayersTime[i].setOutlineColor(sf::Color::Black);
        leaderboardPlayersTime[i].setOutlineThickness(0.5f * preferences.uiScaling);
    }
    mc::Button leaderBoardDoneButton(longButtonTexture, robotoRegular, preferences.uiScaling, "Done");

    sf::Clock elapsedClock;
    elapsedClock.restart();

    sf::Vector2i mousePosition(sf::Mouse::getPosition(window));

    float scrollWhellPosition = 0.f;
    bool leftClickHeld = false;

    int openMenuType = MENU_MAIN;

    while (window.isOpen()) {
        bool volumeChanged = false;
        bool windowSettingsChanged = false;
        bool windowNeedRecreate = false;

        bool leftClick = false;

        bool quit = false;

        sf::String textInputed = "";

        sf::Time elapsedTime = elapsedClock.getElapsedTime();

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    mc::Preferences::saveToFile("settings.dat.gz", preferences);
                    window.close();
                    return 0;
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Escape:
                            if (openMenuType == MENU_SELECTWORLD || openMenuType == MENU_LEADERBOARD || openMenuType == MENU_SETTINGS) {
                                openMenuType = MENU_MAIN;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::TextEntered:
                    textInputed += event.text.unicode;
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
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    switch (event.mouseButton.button) {
                        case sf::Mouse::Left:
                            leftClickHeld = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseWheelScrolled:
                    scrollWhellPosition += event.mouseWheelScroll.delta;
                    break;
                case sf::Event::Resized:
                    preferences.screenSize = sf::Vector2i(event.size.width, event.size.height);
                    screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
                    window.setView(sf::View(screenRect));
                    break;
                default:
                    break;
            }
        }

        switch (openMenuType) {
            case MENU_MAIN:
                if (singleplayerButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    singleplayerButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        worldNameTextBox.setFocused(true);
                        openMenuType = MENU_SELECTWORLD;
                    }
                } else {
                    singleplayerButton.setState(BTN_STATE_NORMAL);
                }
                if (leaderBoardButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    leaderBoardButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        std::ifstream leaderboardFile("leaderboard.json");
                        json leaderboardData = json::parse(leaderboardFile);
                        leaderboardFile.close();
                        std::vector<mc::PlayerAndTime> parsedLeaderboardData;
                        parsedLeaderboardData.resize(leaderboardData.size());
                        for (size_t i = 0; i < leaderboardData.size(); i++) {
                            parsedLeaderboardData[i].playtime = leaderboardData[i][0];
                            std::vector<uint32_t> playerNameUtf32Raw;
                            playerNameUtf32Raw.resize(leaderboardData[i][1].size());
                            for (size_t j = 0; j < leaderboardData[i][1].size(); j++) {
                                playerNameUtf32Raw[j] = leaderboardData[i][1][j];
                            }
                            sf::String playerName = sf::String::fromUtf32(playerNameUtf32Raw.cbegin(), playerNameUtf32Raw.cend());
                            parsedLeaderboardData[i].playerName = playerName;
                        }
                        std::sort(parsedLeaderboardData.begin(), parsedLeaderboardData.end());
                        for (size_t i = 0; i < leaderboardPlayersName.size(); i++) {
                            if (i < leaderboardData.size()) {
                                leaderboardPlayersName[i].setString(sf::String(std::format("{}. ", i + 1) + parsedLeaderboardData[i].playerName));
                                leaderboardPlayersTime[i].setString(std::format("{}m {:02d}s", parsedLeaderboardData[i].playtime / 1200, (parsedLeaderboardData[i].playtime % 1200) / 20));
                            } else {
                                leaderboardPlayersName[i].setString("");
                                leaderboardPlayersTime[i].setString("");
                            }
                        }
                        openMenuType = MENU_LEADERBOARD;
                    }
                } else {
                    leaderBoardButton.setState(BTN_STATE_NORMAL);
                }
                if (optionsButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    optionsButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_SETTINGS;
                    }
                } else {
                    optionsButton.setState(BTN_STATE_NORMAL);
                }
                if (quitGameButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    quitGameButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        quit = true;
                    }
                } else {
                    quitGameButton.setState(BTN_STATE_NORMAL);
                }
                break;
            case MENU_SETTINGS:
                if (masterVolumeSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    masterVolumeSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        masterVolumeSlider.setActiveValue(true);
                        soundEffect.play("click");
                        volumeChanged = true;
                        masterVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.masterVolume = masterVolumeSlider.getValue();
                        masterVolumeSlider.setDisplayText(std::format("Volume: {}%", preferences.masterVolume));
                    } else if (leftClickHeld && masterVolumeSlider.getActiveValue()) {
                        volumeChanged = true;
                        masterVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.masterVolume = masterVolumeSlider.getValue();
                        masterVolumeSlider.setDisplayText(std::format("Volume: {}%", preferences.masterVolume));
                    }
                } else {
                    masterVolumeSlider.setActiveValue(false);
                    masterVolumeSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (musicVolumeSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    musicVolumeSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        musicVolumeSlider.setActiveValue(true);
                        soundEffect.play("click");
                        volumeChanged = true;
                        musicVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.musicVolume = musicVolumeSlider.getValue();
                        musicVolumeSlider.setDisplayText(std::format("Music: {}%", preferences.musicVolume));
                    } else if (leftClickHeld && musicVolumeSlider.getActiveValue()) {
                        volumeChanged = true;
                        musicVolumeSlider.setValueByMousePos(mousePosition);
                        preferences.musicVolume = musicVolumeSlider.getValue();
                        musicVolumeSlider.setDisplayText(std::format("Music: {}%", preferences.musicVolume));
                    }
                } else {
                    musicVolumeSlider.setActiveValue(false);
                    musicVolumeSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (antialiasingLevelSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    antialiasingLevelSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowNeedRecreate = true;
                        antialiasingLevelSlider.setValueByMousePos(mousePosition);
                        preferences.antialiasingLevel = antialiasingLevelSlider.getValue();
                        antialiasingLevelSlider.setDisplayText(std::format("Antialiasing: {}", preferences.antialiasingLevel));
                    }
                } else {
                    antialiasingLevelSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (framerateLimitSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    framerateLimitSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        framerateLimitSlider.setActiveValue(true);
                        soundEffect.play("click");
                        windowSettingsChanged = true;
                        framerateLimitSlider.setValueByMousePos(mousePosition);
                        preferences.framerateLimit = framerateLimitSlider.getValue() * 10;
                        if (preferences.framerateLimit != 0) {
                            framerateLimitSlider.setDisplayText(std::format("Max Framerate: {} fps", preferences.framerateLimit));
                        } else {
                            framerateLimitSlider.setDisplayText("Max Framerate: Unlimited");
                        }
                    } else if (leftClickHeld && framerateLimitSlider.getActiveValue()) {
                        windowSettingsChanged = true;
                        framerateLimitSlider.setValueByMousePos(mousePosition);
                        preferences.framerateLimit = framerateLimitSlider.getValue() * 10;
                        if (preferences.framerateLimit != 0) {
                            framerateLimitSlider.setDisplayText(std::format("Max Framerate: {} fps", preferences.framerateLimit));
                        } else {
                            framerateLimitSlider.setDisplayText("Max Framerate: Unlimited");
                        }
                    }
                } else {
                    framerateLimitSlider.setActiveValue(false);
                    framerateLimitSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (ppbSlider.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    ppbSlider.setState(SLIDER_STATE_HOVERED);
                    if (leftClick) {
                        ppbSlider.setActiveValue(true);
                        soundEffect.play("click");
                        ppbSlider.setValueByMousePos(mousePosition);
                        preferences.gamePixelPerBlock = 0b1 << ppbSlider.getValue();
                        ppbSlider.setDisplayText(std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock));
                    } else if (leftClickHeld && ppbSlider.getActiveValue()) {
                        ppbSlider.setValueByMousePos(mousePosition);
                        preferences.gamePixelPerBlock = 0b1 << ppbSlider.getValue();
                        ppbSlider.setDisplayText(std::format("Pixel Per Block: {}", preferences.gamePixelPerBlock));
                    }
                } else {
                    ppbSlider.setActiveValue(false);
                    ppbSlider.setState(SLIDER_STATE_NORMAL);
                }
                if (fullscreenToggleButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    fullscreenToggleButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowNeedRecreate = true;
                        preferences.fullScreenEnabled ^= 1;
                        fullscreenToggleButton.setDisplayText(preferences.fullScreenEnabled ? "Fullscreen: ON" : "Fullscreen: OFF");
                    }
                } else {
                    fullscreenToggleButton.setState(BTN_STATE_NORMAL);
                }
                if (vsyncToggleButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    vsyncToggleButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        windowSettingsChanged = true;
                        preferences.vsyncEnabled ^= 1;
                        vsyncToggleButton.setDisplayText(preferences.vsyncEnabled ? "VSync: ON" : "VSync: OFF");
                    }
                } else {
                    vsyncToggleButton.setState(BTN_STATE_NORMAL);
                }
                if (uiScalingStepButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    uiScalingStepButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        preferences.uiScaling++;
                        preferences.uiScaling %= 5;
                        preferences.uiScaling = std::clamp(preferences.uiScaling, 1, 4);
                        uiScalingStepButton.setDisplayText(std::format("GUI Scale: {}", preferences.uiScaling));
                    }
                } else {
                    uiScalingStepButton.setState(BTN_STATE_NORMAL);
                }
                if (settingsDoneButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    settingsDoneButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_MAIN;
                    }
                } else {
                    settingsDoneButton.setState(BTN_STATE_NORMAL);
                }
                break;
            case MENU_SELECTWORLD:
                if (leftClick) {
                    if (worldNameTextBox.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                        worldNameTextBox.setFocused(true);
                    } else {
                        worldNameTextBox.setFocused(false);
                    }
                }
                if (worldNameTextBox.getFocused()) {
                    worldNameTextBox.setDisplayString(worldNameTextBox.getDisplayString() + textInputed);
                }
                if (selectWorldDoneButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    selectWorldDoneButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        if (worldNameTextBox.getDisplayString().toAnsiString().size() > 0) {
                            musicPlayer.stop();
                            openMenuType = MENU_MAIN;
                            windowSettingsChanged =  true;
                            volumeChanged = true;
                            game(worldNameTextBox.getDisplayString().toAnsiString(), window, screenRect, soundEffect, preferences);
                            screenRect = sf::FloatRect(0, 0, window.getSize().x, window.getSize().y);
                            musicPlayer.start();
                            if (!window.isOpen()) {
                                quit = true;
                            }
                        }
                    }
                } else {
                    selectWorldDoneButton.setState(BTN_STATE_NORMAL);
                }
                break;
            case MENU_LEADERBOARD:
                if (leaderBoardDoneButton.getGlobalBounds().contains(sf::Vector2f(mousePosition))) {
                    leaderBoardDoneButton.setState(BTN_STATE_HOVERED);
                    if (leftClick) {
                        soundEffect.play("click");
                        openMenuType = MENU_MAIN;
                    }
                } else {
                    leaderBoardDoneButton.setState(BTN_STATE_NORMAL);
                }
                break;
            default:
                break;
        }

        if (windowSettingsChanged) {
            window.setFramerateLimit(preferences.framerateLimit);
            window.setVerticalSyncEnabled(preferences.vsyncEnabled);
        }

        if (windowNeedRecreate) {
            ctxSettings.antialiasingLevel = preferences.antialiasingLevel;
            if (preferences.fullScreenEnabled) {
                sf::VideoMode fullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
                window.create(fullScreenVideoMode, "Minecraft SFML Edition", sf::Style::Default | sf::Style::Fullscreen, ctxSettings);
                screenRect = sf::FloatRect(0, 0, fullScreenVideoMode.width, fullScreenVideoMode.height);
                window.setView(sf::View(screenRect));
            } else {
                screenRect = sf::FloatRect(0.f, 0.f, preferences.screenSize.x, preferences.screenSize.y);
                window.create(sf::VideoMode(static_cast<int>(round(screenRect.width)), static_cast<int>(round(screenRect.height))), "Minecraft SFML Edition", sf::Style::Default, ctxSettings);
                window.setView(sf::View(screenRect));
            }
            window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
            window.setFramerateLimit(preferences.framerateLimit);
            window.setVerticalSyncEnabled(preferences.vsyncEnabled);
            window.setKeyRepeatEnabled(false);
        }

        if (volumeChanged) {
            sf::Listener::setGlobalVolume(static_cast<float>(preferences.masterVolume));
            musicPlayer.setVolume(static_cast<float>(preferences.musicVolume) / 100.f);
        }

        dirtBackground.setTextureRect(sf::IntRect(0, 0, static_cast<int>(std::ceil(screenRect.width / (preferences.uiScaling * 2))), static_cast<int>(std::ceil(screenRect.height / (preferences.uiScaling * 2)))));
        dirtBackground.setScale(sf::Vector2f(preferences.uiScaling * 2, preferences.uiScaling * 2));

        switch (openMenuType) {
            case MENU_MAIN:
                titleSprite.setScale(sf::Vector2f(static_cast<float>(preferences.uiScaling) / 7.5f, static_cast<float>(preferences.uiScaling) / 7.5f));
                singleplayerButton.setScaling(preferences.uiScaling);
                leaderBoardButton.setScaling(preferences.uiScaling);
                optionsButton.setScaling(preferences.uiScaling);
                quitGameButton.setScaling(preferences.uiScaling);
                versionText.setCharacterSize(12 * preferences.uiScaling);
                versionText.setOutlineThickness(0.5f * preferences.uiScaling);
                copyleftText.setCharacterSize(12 * preferences.uiScaling);
                copyleftText.setOutlineThickness(0.5f * preferences.uiScaling);
                menuBackground.setScale(screenRect.height / static_cast<float>(menuBackgroundTexture.getSize().y));

                titleSprite.setPosition(sf::Vector2f(screenRect.width / 2.f - titleSprite.getGlobalBounds().width / 2.f, screenRect.height * 0.10f));
                singleplayerButton.setPosition(sf::Vector2f(screenRect.width / 2.f - singleplayerButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f));
                leaderBoardButton.setPosition(sf::Vector2f(screenRect.width / 2.f - leaderBoardButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f + static_cast<float>(preferences.uiScaling * 25)));
                optionsButton.setPosition(sf::Vector2f(screenRect.width / 2.f - optionsButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                quitGameButton.setPosition(sf::Vector2f(screenRect.width / 2.f - quitGameButton.getGlobalBounds().width / 2.f, screenRect.height * 0.4f + static_cast<float>(preferences.uiScaling * 25) * 3.5f));
                versionText.setPosition(sf::Vector2f(5.f, screenRect.height - 16.f * preferences.uiScaling));
                copyleftText.setOrigin(sf::Vector2f(copyleftText.getGlobalBounds().width, 0.f));
                copyleftText.setPosition(sf::Vector2f(screenRect.width - 5.f, screenRect.height - 16.f * preferences.uiScaling));

                technobladeNeverDiesText.setCharacterSize(11 * preferences.uiScaling);
                technobladeNeverDiesText.setOutlineThickness(0.5f * preferences.uiScaling);
                technobladeNeverDiesText.setOrigin(sf::Vector2f(technobladeNeverDiesText.getLocalBounds().width / 2.f, technobladeNeverDiesText.getLocalBounds().height / 2.f));
                technobladeNeverDiesText.setPosition(sf::Vector2f(titleSprite.getGlobalBounds().left + titleSprite.getGlobalBounds().width * 0.95f, titleSprite.getGlobalBounds().top + titleSprite.getGlobalBounds().height * 0.65f));
                break;
            case MENU_SETTINGS:
                optionText.setCharacterSize(12 * preferences.uiScaling);
                optionText.setOutlineThickness(0.5f * preferences.uiScaling);
                masterVolumeSlider.setScaling(preferences.uiScaling);
                musicVolumeSlider.setScaling(preferences.uiScaling);
                antialiasingLevelSlider.setScaling(preferences.uiScaling);
                framerateLimitSlider.setScaling(preferences.uiScaling);
                fullscreenToggleButton.setScaling(preferences.uiScaling);
                vsyncToggleButton.setScaling(preferences.uiScaling);
                uiScalingStepButton.setScaling(preferences.uiScaling);
                ppbSlider.setScaling(preferences.uiScaling);
                settingsDoneButton.setScaling(preferences.uiScaling);

                optionText.setPosition(sf::Vector2f(screenRect.width / 2.f - optionText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                masterVolumeSlider.setPosition(sf::Vector2f(screenRect.width / 2.f - masterVolumeSlider.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f));
                musicVolumeSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f));
                antialiasingLevelSlider.setPosition(sf::Vector2f(screenRect.width / 2.f - antialiasingLevelSlider.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25)));
                framerateLimitSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25)));
                fullscreenToggleButton.setPosition(sf::Vector2f(screenRect.width / 2.f - fullscreenToggleButton.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                vsyncToggleButton.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 2.f));
                uiScalingStepButton.setPosition(sf::Vector2f(screenRect.width / 2.f - uiScalingStepButton.getGlobalBounds().width - 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 3.f));
                ppbSlider.setPosition(sf::Vector2f(screenRect.width / 2.f + 5.f * preferences.uiScaling, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 3.f));
                settingsDoneButton.setPosition(sf::Vector2f(screenRect.width / 2.f - settingsDoneButton.getGlobalBounds().width / 2.f, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 4.5f));

                masterVolumeSlider.setValue(preferences.masterVolume);
                musicVolumeSlider.setValue(preferences.musicVolume);
                antialiasingLevelSlider.setValue(preferences.antialiasingLevel);
                framerateLimitSlider.setValue(preferences.framerateLimit / 10);
                ppbSlider.setValue(static_cast<int>(std::round(log2(preferences.gamePixelPerBlock))));
                break;
            case MENU_SELECTWORLD:
                selectWorldText.setCharacterSize(12 * preferences.uiScaling);
                selectWorldText.setOutlineThickness(0.5f * preferences.uiScaling);
                worldNameTextBox.setScaling(preferences.uiScaling);
                selectWorldDoneButton.setScaling(preferences.uiScaling);

                selectWorldText.setPosition(sf::Vector2f(screenRect.width / 2.f - selectWorldText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                worldNameTextBox.setPosition(sf::Vector2f(screenRect.width / 2.f - worldNameTextBox.getGlobalBounds().width / 2.f, screenRect.height * 0.35f));
                selectWorldDoneButton.setPosition(sf::Vector2f(screenRect.width / 2.f - selectWorldDoneButton.getGlobalBounds().width / 2.f, screenRect.height * 0.35f + static_cast<float>(preferences.uiScaling * 25) * 1.5f));
                break;
            case MENU_LEADERBOARD:
                leaderboardText.setCharacterSize(12 * preferences.uiScaling);
                leaderboardText.setOutlineThickness(0.5f * preferences.uiScaling);
                for (size_t i = 0; i < leaderboardPlayersName.size(); i++) {
                    leaderboardPlayersName[i].setCharacterSize(12 * preferences.uiScaling);
                    leaderboardPlayersName[i].setOutlineThickness(0.5f * preferences.uiScaling);
                    leaderboardPlayersTime[i].setCharacterSize(12 * preferences.uiScaling);
                    leaderboardPlayersTime[i].setOutlineThickness(0.5f * preferences.uiScaling);
                }
                leaderBoardDoneButton.setScaling(preferences.uiScaling);

                leaderboardText.setPosition(sf::Vector2f(screenRect.width / 2.f - leaderboardText.getGlobalBounds().width / 2.f, screenRect.height * 0.075f));
                for (size_t i = 0; i < leaderboardPlayersName.size(); i++) {
                    leaderboardPlayersName[i].setPosition(sf::Vector2f(screenRect.width / 2.f - preferences.uiScaling * 200.f, screenRect.height * 0.2f + static_cast<float>(preferences.uiScaling * 25) * i));
                    leaderboardPlayersTime[i].setPosition(sf::Vector2f(screenRect.width / 2.f + preferences.uiScaling * (200.f - 15.f * preferences.uiScaling), screenRect.height * 0.2f + static_cast<float>(preferences.uiScaling * 25) * i));
                }
                leaderBoardDoneButton.setPosition(sf::Vector2f(screenRect.width / 2.f - leaderBoardDoneButton.getGlobalBounds().width / 2.f, screenRect.height * 0.25f + static_cast<float>(preferences.uiScaling * 25) * 8.f));
            default:
                break;
        }

        if (quit) {
            mc::Preferences::saveToFile("settings.dat.gz", preferences);
            window.close();
            return 0;
        }

        musicPlayer.update();

        window.clear(sf::Color::Black);

        switch (openMenuType) {
            case MENU_MAIN:
                menuBackground.setPosition(sf::Vector2f(-mod(elapsedTime.asSeconds() * 20.f * (screenRect.height / static_cast<float>(menuBackgroundTexture.getSize().y)), static_cast<float>(menuBackgroundTexture.getSize().x) * screenRect.height / static_cast<float>(menuBackgroundTexture.getSize().y)), 0.f));
                technobladeNeverDiesText.setScale(sf::Vector2f(1.075f - 0.075f * sin(mod(elapsedTime.asSeconds() * 3.14159265358979f * 2.f, 3.14159265358979f)), 1.075f - 0.075f * sin(mod(elapsedTime.asSeconds() * 3.14159265358979f * 2.f, 3.14159265358979f))));
                window.draw(menuBackground);
                window.draw(titleSprite);
                window.draw(technobladeNeverDiesText);
                window.draw(singleplayerButton);
                window.draw(leaderBoardButton);
                window.draw(optionsButton);
                window.draw(quitGameButton);
                window.draw(versionText);
                window.draw(copyleftText);
                break;
            case MENU_SELECTWORLD:
                window.draw(dirtBackground);
                window.draw(selectWorldText);
                window.draw(worldNameTextBox);
                window.draw(selectWorldDoneButton);
                break;
            case MENU_LEADERBOARD:
                window.draw(dirtBackground);
                window.draw(leaderboardText);
                for (size_t i = 0; i < leaderboardPlayersName.size(); i++) {
                    window.draw(leaderboardPlayersName[i]);
                    window.draw(leaderboardPlayersTime[i]);
                }
                window.draw(leaderBoardDoneButton);
                break;
            case MENU_SETTINGS:
                window.draw(dirtBackground);
                window.draw(optionText);
                window.draw(masterVolumeSlider);
                window.draw(musicVolumeSlider);
                window.draw(antialiasingLevelSlider);
                window.draw(framerateLimitSlider);
                window.draw(fullscreenToggleButton);
                window.draw(vsyncToggleButton);
                window.draw(uiScalingStepButton);
                window.draw(ppbSlider);
                window.draw(settingsDoneButton);
                break;
        }

        window.display();
    }

    return 0;
}