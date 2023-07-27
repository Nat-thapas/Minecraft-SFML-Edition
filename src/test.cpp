#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode(1600U, 900U), "SFML works! or does it?", sf::Style::Titlebar | sf::Style::Close);
    // window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);

    sf::CircleShape testCircle(100.f, 200ULL);
    testCircle.setFillColor(sf::Color::Red);

    sf::Font roboto;
    if (!roboto.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
        std::cout << "Font failed to load" << std::endl;
    }

    sf::Text fpsCounter;
    fpsCounter.setFont(roboto);
    fpsCounter.setCharacterSize(24U);
    fpsCounter.setFillColor(sf::Color::White);
    fpsCounter.setOutlineThickness(1.f);
    fpsCounter.setOutlineColor(sf::Color::Black);

    sf::Text cpuTimeDisplay;
    cpuTimeDisplay.setFont(roboto);
    cpuTimeDisplay.setCharacterSize(24U);
    cpuTimeDisplay.setFillColor(sf::Color::White);
    cpuTimeDisplay.setOutlineThickness(1.f);
    cpuTimeDisplay.setOutlineColor(sf::Color::Black);
    cpuTimeDisplay.setPosition(sf::Vector2f(0.f, 24.f));

    sf::Text gpuTimeDisplay;
    gpuTimeDisplay.setFont(roboto);
    gpuTimeDisplay.setCharacterSize(24U);
    gpuTimeDisplay.setFillColor(sf::Color::White);
    gpuTimeDisplay.setOutlineThickness(1.f);
    gpuTimeDisplay.setOutlineColor(sf::Color::Black);
    gpuTimeDisplay.setPosition(sf::Vector2f(0.f, 48.f));

    sf::Clock frameTimeClock;
    sf::Clock cpuTimeClock;
    sf::Clock gpuTimeClock;

    float baseRadius = 50.f;
    float radius;
    sf::Vector2f position(200.f, 200.f);
    sf::Vector2f speed(0.f, 0.f);
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
                            speed.y += -1.f;
                            break;
                        case sf::Keyboard::A:
                            speed.x += -1.f;
                            break;
                        case sf::Keyboard::S:
                            speed.y += 1.f;
                            break;
                        case sf::Keyboard::D:
                            speed.x += 1.f;
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::KeyReleased:
                    switch(event.key.code) {
                        case sf::Keyboard::W:
                            speed.y -= -1.f;
                            break;
                        case sf::Keyboard::A:
                            speed.x -= -1.f;
                            break;
                        case sf::Keyboard::S:
                            speed.y -= 1.f;
                            break;
                        case sf::Keyboard::D:
                            speed.x -= 1.f;
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
        radius = baseRadius*baseRadius/100.f;
        position.x += speed.x * frameTimeMicrosecond/1000.f;
        position.y += speed.y * frameTimeMicrosecond/1000.f;
        testCircle.setRadius(radius);
        testCircle.setOrigin(sf::Vector2f(radius, radius));
        testCircle.setPosition(position);

        // End of CPU time measurement
        cpuTimeMicrosecond = cpuTimeClock.getElapsedTime().asMicroseconds();

        // Start of GPU time measurement
        gpuTimeClock.restart();

        // Rendering
        window.clear(sf::Color(160, 192, 255));
        window.draw(testCircle);
        window.draw(fpsCounter);
        window.draw(cpuTimeDisplay);
        window.draw(gpuTimeDisplay);
        window.display();
        
        // End of GPU time measurement
        gpuTimeMicrosecond = gpuTimeClock.getElapsedTime().asMicroseconds();
    }

    return 0;
}