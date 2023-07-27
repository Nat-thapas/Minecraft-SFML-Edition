#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "cmod.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 960), "Minecraft!", sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(300);
    return 0;

    sf::Font robotoRegular;
    if (!robotoRegular.loadFromFile("resources/fonts/Roboto-Regular.ttf")) {
        std::cout << "Font: Roboto Regular failed to load" << std::endl;
        exit(1);
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
}