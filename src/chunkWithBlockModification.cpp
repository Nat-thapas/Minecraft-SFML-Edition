#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <ctime>

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
    // "resources/textures/atlases/blocksAtlas.png", "resources/textures/atlases/blocksAtlas.json"
    sf::Font robotoRegular;
    robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf");

    mc::PerfDebugInfo perfDebugInfo(sf::Vector2f(0.f, 0.f), robotoRegular, 24, sf::Color::White, sf::Color::Black, 1.f);
    sf::Clock elapsedClock;

    int b = 0;
    int xp = 0;
    int lastTick = 0;
    
    while (window.isOpen())
    {
        perfDebugInfo.startFrame();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        perfDebugInfo.endEventLoop();
        perfDebugInfo.endPlayerInputProcessing();
        perfDebugInfo.endRandomTick();

        if (elapsedClock.getElapsedTime().asMilliseconds() - lastTick > 50) {
            lastTick += 50;
            chunk.tickAnimation();
            chunk2.tickAnimation();
            b++;
            b = mod(b, 200);
            chunk.breakBlock(4, 191, &xp);
            chunk.placeBlock(4, 191, idiv(b, 4));
        }

        perfDebugInfo.endChunksUpdate();
        perfDebugInfo.endEntitiesProcessing();

        window.clear(sf::Color(160, 192, 255));
        window.draw(chunk);
        window.draw(chunk2);

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