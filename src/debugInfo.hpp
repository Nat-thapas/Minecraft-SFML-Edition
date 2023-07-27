#ifndef DEBUGINFO_HPP
#define DEBUGINFO_HPP

#include <SFML/Graphics.hpp>

class DebugInfo : public sf::Drawable {
    sf::Clock frameTimeClock;
    sf::Clock cpuTimeClock;
    sf::Clock gpuTimeClock;
    long long frameTime;
    long long cpuTime;
    long long gpuTime;
    sf::Font font;
    sf::Text fpsLabel;
    sf::Text cpuTimeLabel;
    sf::Text gpuTimeLabel;
    sf::Vector2f position;
    unsigned int characterSize;
    float outlineThickness;
    sf::Color fillColor;
    sf::Color outlineColor;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    DebugInfo(sf::Vector2f position, sf::Font font, unsigned int characterSize, float outlineThickness, sf::Color fillColor, sf::Color outlineColor);
    void startCpuTime();
    void endCpuTime();
    void startGpuTime();
    void endGpuTime();
    void endFrame();
    void update();
};

#endif // DEBUGINFO_HPP