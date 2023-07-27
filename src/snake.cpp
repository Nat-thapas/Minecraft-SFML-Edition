#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <format>
#include <iostream>
#include <string>
#include <vector>

int cmod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1600U, 900U), "SNAKE! SFML Edition", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(3U);

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

    sf::Text scoreDisplay;
    scoreDisplay.setFont(robotoRegular);
    scoreDisplay.setCharacterSize(48U);
    scoreDisplay.setFillColor(sf::Color::White);
    scoreDisplay.setOutlineThickness(1.f);
    scoreDisplay.setOutlineColor(sf::Color::Black);
    scoreDisplay.setPosition(sf::Vector2f(1250.f, 10.f));

    sf::Text gameOverText;
    gameOverText.setFont(robotoRegular);
    gameOverText.setCharacterSize(112U);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOutlineThickness(5.f);
    gameOverText.setOutlineColor(sf::Color::Black);
    gameOverText.setString("GAME OVER");
    sf::FloatRect gameOverTextLocalBound = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gameOverTextLocalBound.width/2.f, gameOverTextLocalBound.height/2.f);
    gameOverText.setPosition(sf::Vector2f(800.f, 450.f));

    sf::RectangleShape snakeBlockRenderer;
    snakeBlockRenderer.setFillColor(sf::Color::Green);
    snakeBlockRenderer.setSize(sf::Vector2f(50.f, 50.f));

    sf::RectangleShape appleBlockRenderer;
    appleBlockRenderer.setFillColor(sf::Color::Red);
    appleBlockRenderer.setSize(sf::Vector2f(50.f, 50.f));

    sf::Clock frameTimeClock;
    sf::Clock cpuTimeClock;
    sf::Clock gpuTimeClock;

    float fps = 0.f;
    long long int frameTimeMicrosecond;
    long long int cpuTimeMicrosecond;
    long long int gpuTimeMicrosecond;

    int direction = -1;  // -1 = GAME NOT STARTED, 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT
    sf::Vector2i headPos(7, 8);
    sf::Vector2i tailPos;
    std::deque<sf::Vector2i> snakeCoords;
    snakeCoords.push_front(headPos);
    int snakeLen = 1;
    int newSnakeLen = 1;

    // Initializing the game grid
    // 0 = EMPTY, 'S' = SNAKE, 'A' = APPLE
    sf::Vector2i applePos(23, 9);
    char grid[32][18];
    for (int x=0; x<32; x++) {
        for (int y=0; y<18; y++) {
            grid[x][y] = 0;
        }
    }
    grid[applePos.x][applePos.y] = 'A';
    grid[headPos.x][headPos.y] = 'S';

    bool gameEnded = false;

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
        // std::cout << "Framtime: " << frameTimeMicrosecond / 1000.f << " ms." << std::endl;
        // std::cout << "Framerate (rolling average): " << fps << " FPS" << std::endl;

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
                    switch(event.key.code) {
                        case sf::Keyboard::W:
                            direction = 0;
                            break;
                        case sf::Keyboard::A:
                            direction = 3;
                            break;
                        case sf::Keyboard::S:
                            direction = 2;
                            break;
                        case sf::Keyboard::D:
                            direction = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        // Processing
        switch (direction) {
            case 0:
                headPos.y--;
                break;
            case 1:
                headPos.x++;
                break;
            case 2:
                headPos.y++;
                break;
            case 3:
                headPos.x--;
                break;
        }

        if (!gameEnded) {
            headPos.x = cmod(headPos.x, 32);
            headPos.y = cmod(headPos.y, 18);
            snakeCoords.push_front(headPos);
            if (grid[headPos.x][headPos.y] == 'S' && direction != -1) {
                gameEnded = true;
            }
            if (grid[headPos.x][headPos.y] == 'A') {
                newSnakeLen++;
                do {
                    applePos.x = rand()%32;
                    applePos.y = rand()%18;
                } while (grid[applePos.x][applePos.y] != 0);
                grid[applePos.x][applePos.y] = 'A';
            }
            if (newSnakeLen <= snakeLen) {
                tailPos = snakeCoords.back();
                snakeCoords.pop_back();
                grid[tailPos.x][tailPos.y] = 0;
                snakeLen--;
            }
            grid[headPos.x][headPos.y] = 'S';
            snakeLen++;
        }

        #ifndef __INTELLISENSE__
            scoreDisplay.setString(std::format("SCORE: {:06d}", snakeLen-1));
        #endif

        // End of CPU time measurement
        cpuTimeMicrosecond = cpuTimeClock.getElapsedTime().asMicroseconds();

        // Start of GPU time measurement
        gpuTimeClock.restart();

        // Rendering
        window.clear(sf::Color::Black);
        // Snake rendering
        for (sf::Vector2i pos : snakeCoords) {
            snakeBlockRenderer.setPosition(sf::Vector2f(pos.x*50, pos.y*50));
            window.draw(snakeBlockRenderer);
        }
        // Apple rendering
        appleBlockRenderer.setPosition(sf::Vector2f(applePos.x*50, applePos.y*50));
        window.draw(appleBlockRenderer);

        // Draw overlays
        window.draw(fpsCounter);
        window.draw(cpuTimeDisplay);
        window.draw(gpuTimeDisplay);
        window.draw(scoreDisplay);
        if (gameEnded) {
            window.draw(gameOverText);
        }

        window.display();
        
        // End of GPU time measurement
        gpuTimeMicrosecond = gpuTimeClock.getElapsedTime().asMicroseconds();
    }

    return 0;
}