#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include "cmod.hpp"

const float cameraMoveSpeedMultiplier = 4.f;

int main() {
    sf::RenderWindow window(sf::VideoMode(1600U, 960U), "Minecraft!", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(120U);

    sf::Font robotoRegular;
    if (!robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
        std::cout << "Font failed to load" << std::endl;
    }

    sf::Text fpsCounter;
    fpsCounter.setFont(robotoRegular);
    fpsCounter.setCharacterSize(24U);
    fpsCounter.setFillColor(sf::Color::White);
    fpsCounter.setOutlineThickness(1.f);
    fpsCounter.setOutlineColor(sf::Color::Black);

    sf::Text cpuTimeDisplay;
    cpuTimeDisplay.setFont(robotoRegular);
    cpuTimeDisplay.setCharacterSize(24U);
    cpuTimeDisplay.setFillColor(sf::Color::White);
    cpuTimeDisplay.setOutlineThickness(1.f);
    cpuTimeDisplay.setOutlineColor(sf::Color::Black);
    cpuTimeDisplay.setPosition(sf::Vector2f(0.f, 24.f));

    sf::Text gpuTimeDisplay;
    gpuTimeDisplay.setFont(robotoRegular);
    gpuTimeDisplay.setCharacterSize(24U);
    gpuTimeDisplay.setFillColor(sf::Color::White);
    gpuTimeDisplay.setOutlineThickness(1.f);
    gpuTimeDisplay.setOutlineColor(sf::Color::Black);
    gpuTimeDisplay.setPosition(sf::Vector2f(0.f, 48.f));

    // Initializing game grid array
    unsigned char gameGrid[25][15] = {
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 5, 5, 5, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 5, 5, 5, 5, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 4, 4, 4, 4, 5, 5, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 5, 5, 5, 5, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 5, 5, 5, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 3, 3, 3, 2, 0, 0, 0, 0, 0, 0, 0}
    };

    // Blocks drawer
    sf::Sprite drawer;
    drawer.setScale(4.f, 4.f);

    // Player
    sf::Sprite player;
    player.setScale(4.f, 4.f);
    player.setOrigin(0.f, 32.f);

    // Block highlighter
    sf::RectangleShape highlighter;
    highlighter.setSize(sf::Vector2f(62.f, 62.f));
    highlighter.setOrigin(-1.f, -1.f);
    highlighter.setFillColor(sf::Color(0, 0, 0, 0));
    highlighter.setOutlineThickness(2.f);
    highlighter.setOutlineColor(sf::Color::Black);

    // Textures loading
    std::vector<sf::Texture> textures;
    sf::Texture texture;
    for (int i=0; i<6; i++) {
        if (!texture.loadFromFile(std::format("resources/textures/blocks/{}.png", i))) {
            std::cout << "Texture at resources/textures/blocks/" << i << ".png failed to load" << std::endl;
        }
        textures.push_back(texture);
    }
    sf::Texture missingTexture;
    if (!missingTexture.loadFromFile("resources/textures/blocks/256.png")) {
        std::cout << "Missing texture failed to load" << std::endl;
    }
    sf::Texture playerLeft;
    if (!playerLeft.loadFromFile("resources/textures/players/left.png")) {
        std::cout << "Player left texture failed to load" << std::endl;
    }
    sf::Texture playerRight;
    if (!playerRight.loadFromFile("resources/textures/players/right.png")) {
        std::cout << "Player right texture failed to load" << std::endl;
    }
    player.setTexture(playerRight);

    sf::Vector2i clickedPos;
    sf::Vector2i mousePos;
    unsigned char selectedBlock = 3;
    sf::Vector2f cameraMoveSpeed(0.f, 0.f);
    sf::Vector2f cameraPos(0.f, 0.f);

    int currentTerrainHeight;
    int playerBlockPosX;

    sf::Clock frameTimeClock;
    sf::Clock cpuTimeClock;
    sf::Clock gpuTimeClock;

    float fps = 0.f;
    long long int frameTimeMicrosecond;
    long long int cpuTimeMicrosecond;
    long long int gpuTimeMicrosecond;

    // Game loop
    while (window.isOpen()) {
        // Start of CPU time measurement
        cpuTimeClock.restart();

        // Get frametime delta
        frameTimeMicrosecond = frameTimeClock.restart().asMicroseconds();
        if (frameTimeMicrosecond <= 0) {
            frameTimeMicrosecond = 1;
        }

        // Debug info
        fps = (fps + 1000000.f / frameTimeMicrosecond) / 2.f;

        // Intellisense is broken on c++20 std::format, https://github.com/microsoft/vscode-cpptools/issues/11038
        // and will throw an error when there is none
        // This issue is supposed to be fixed in 1.17.0
        #ifndef __INTELLISENSE__
            fpsCounter.setString(std::format("{:.2f} FPS", fps));
            cpuTimeDisplay.setString(std::format("CPU: {:.2f} ms.", cpuTimeMicrosecond/1000.f));
            gpuTimeDisplay.setString(std::format("GPU: {:.2f} ms.", gpuTimeMicrosecond/1000.f));
        #endif

        // Event loop
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Num0:
                            selectedBlock = 0;
                            break;
                        case sf::Keyboard::Num1:
                            selectedBlock = 1;
                            break;
                        case sf::Keyboard::Num2:
                            selectedBlock = 2;
                            break;
                        case sf::Keyboard::Num3:
                            selectedBlock = 3;
                            break;
                        case sf::Keyboard::Num4:
                            selectedBlock = 4;
                            break;
                        case sf::Keyboard::Num5:
                            selectedBlock = 5;
                            break;
                        case sf::Keyboard::W:
                            cameraMoveSpeed.y++;
                            break;
                        case sf::Keyboard::A:
                            cameraMoveSpeed.x++;
                            break;
                        case sf::Keyboard::S:
                            cameraMoveSpeed.y--;
                            break;
                        case sf::Keyboard::D:
                            cameraMoveSpeed.x--;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch (event.key.code) {
                        case sf::Keyboard::W:
                            cameraMoveSpeed.y--;
                            break;
                        case sf::Keyboard::A:
                            cameraMoveSpeed.x--;
                            break;
                        case sf::Keyboard::S:
                            cameraMoveSpeed.y++;
                            break;
                        case sf::Keyboard::D:
                            cameraMoveSpeed.x++;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        clickedPos.x = event.mouseButton.x - cameraPos.x;
                        clickedPos.y = event.mouseButton.y - cameraPos.y;
                        clickedPos = clickedPos/64;
                        gameGrid[clickedPos.x][14-clickedPos.y] = 0;
                    } else if (event.mouseButton.button == sf::Mouse::Right) {
                        clickedPos.x = event.mouseButton.x - cameraPos.x;
                        clickedPos.y = event.mouseButton.y - cameraPos.y;
                        clickedPos = clickedPos/64;
                        gameGrid[clickedPos.x][14-clickedPos.y] = selectedBlock;
                    }
                    break;
                default:
                    break;
            }
        }

        // Processing
        cameraPos += cameraMoveSpeed * (cameraMoveSpeedMultiplier * frameTimeMicrosecond / 10000);
        mousePos = sf::Mouse::getPosition(window);
        mousePos.x -= cmod(cameraPos.x, 64);
        mousePos.y -= cmod(cameraPos.y, 64);
        mousePos /= 64;
        highlighter.setPosition(mousePos.x*64.f+cmod(cameraPos.x, 64), mousePos.y*64.f+cmod(cameraPos.y, 64));

        if (cameraMoveSpeed.x > 0) {
            player.setTexture(playerLeft);
        } else if (cameraMoveSpeed.x < 0) {
            player.setTexture(playerRight);
        }

        playerBlockPosX = (768.f + 32.f - cameraPos.x) / 64;

        for (int i=14; i>=0; i--) {
            if (gameGrid[playerBlockPosX][i] != 0) {
                currentTerrainHeight = i + 1;
                break;
            }
        }

        player.setPosition(784.f, 960.f - currentTerrainHeight*64.f);

        // End of CPU time measurement
        cpuTimeMicrosecond = cpuTimeClock.getElapsedTime().asMicroseconds();

        // Start of GPU time measurement
        gpuTimeClock.restart();

        // Rendering
        window.clear(sf::Color(160, 192, 255));

        // Drawing tiles
        for (int x=0; x<25; x++) {
            for (int y=0; y<15; y++) {
                drawer.setPosition(x*64.f+cameraPos.x, (14-y)*64.f+cameraPos.y);
                if (gameGrid[x][y] < 6) {
                    drawer.setTexture(textures.at(gameGrid[x][y]));
                } else {
                    drawer.setTexture(missingTexture);
                }
                window.draw(drawer);
            }
        }

        window.draw(highlighter);
        window.draw(player);

        // Draw overlays
        window.draw(fpsCounter);
        window.draw(cpuTimeDisplay);
        window.draw(gpuTimeDisplay);

        window.display();
        
        // End of GPU time measurement
        gpuTimeMicrosecond = gpuTimeClock.getElapsedTime().asMicroseconds();
    }

    return 0;
}