#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "mod.hpp"
#include "mc_PerfDebugInfo.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "SNAKE! SFML Edition", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(6);

    sf::Font robotoRegular;
    if (!robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
        std::cout << "Font failed to load" << std::endl;
        exit(1);
    }

    mc::PerfDebugInfo debugInfo(sf::Vector2f(5.f, 5.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);

    sf::Text scoreDisplay;
    scoreDisplay.setFont(robotoRegular);
    scoreDisplay.setCharacterSize(48);
    scoreDisplay.setFillColor(sf::Color::White);
    scoreDisplay.setOutlineThickness(1.f);
    scoreDisplay.setOutlineColor(sf::Color::Black);
    scoreDisplay.setPosition(sf::Vector2f(1250.f, 10.f));

    sf::Text gameOverText;
    gameOverText.setFont(robotoRegular);
    gameOverText.setCharacterSize(112);
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
    bool renderDebugInfo = false;

    sf::Event event;

    // Game loop
    while (window.isOpen()) {
        debugInfo.startFrame();

        // Event loop
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
                        case sf::Keyboard::F3:
                            renderDebugInfo ^= 1;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        debugInfo.endEventLoop();

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

        debugInfo.endPlayerInputProcessing();
        debugInfo.endRandomTick();

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

        debugInfo.endChunksUpdate();

        #ifndef __INTELLISENSE__
            scoreDisplay.setString(std::format("SCORE: {:06d}", snakeLen-1));
        #endif

        debugInfo.endEntitiesProcessing();

        // Rendering
        window.clear(sf::Color::Black);
        // Snake rendering
        for (sf::Vector2i pos : snakeCoords) {
            snakeBlockRenderer.setPosition(sf::Vector2f(pos.x*50, pos.y*50));
            window.draw(snakeBlockRenderer);
        }

        debugInfo.endChunksRendering();

        // Apple rendering
        appleBlockRenderer.setPosition(sf::Vector2f(applePos.x*50, applePos.y*50));
        window.draw(appleBlockRenderer);

        debugInfo.endEntitiesRendering();
        debugInfo.endParticlesRendering();

        // Draw overlays
        window.draw(scoreDisplay);
        if (gameEnded) {
            window.draw(gameOverText);
        }
        if (renderDebugInfo) {
            window.draw(debugInfo);
        }
        debugInfo.endOverlaysRendering();

        window.display();
        
        debugInfo.updateLabels();
        debugInfo.endFrame();
    }

    return 0;
}