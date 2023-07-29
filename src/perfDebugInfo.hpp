#ifndef PERFDEBUGINFO_HPP
#define PERFDEBUGINFO_HPP

#include <SFML/Graphics.hpp>

namespace mc {

class DebugInfo : public sf::Drawable {
    sf::Clock frameTimeClock;
    sf::Clock clock;
    sf::Font font;
    sf::Vector2f position;
    unsigned characterSize;
    sf::Color fillColor;
    sf::Color outlineColor;
    float outlineThickness;
    float lineHeightMultiplier;
    long long frameTime;
    long long eventLoopTime;
    long long playerInputProcessingTime;
    long long randomTickTime;
    long long chunksUpdateTime;
    long long entitiesProcessingTime;
    long long chunksRenderingTime;
    long long entitiesRenderingTime;
    long long particlesRenderingTime;
    long long overlaysRenderingTime;
    sf::Text fpsLabel;
    sf::Text eventLoopTimeLabel;
    sf::Text playerInputProcessingTimeLabel;
    sf::Text randomTickTimeLabel;
    sf::Text chucksUpdateTimeLabel;
    sf::Text entitiesProcessingTimeLabel;
    sf::Text chunksRenderingTimeLabel;
    sf::Text entitiesRenderingTimeLabel;
    sf::Text particlesRenderingTimeLabel;
    sf::Text overlaysRenderingTimeLabel;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    void setPosition(sf::Vector2f&);
    void setFont(sf::Font&);
    void setCharacterSize(unsigned);
    void setFillColor(sf::Color&);
    void setOutlineColor(sf::Color&);
    void setOutlineThickness(float);
    void setLineHeightMultiplier(float);
    void updateSettings();
    DebugInfo(sf::Vector2f position, sf::Font font, unsigned characterSize, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, float lineHeightMultiplier = 1.2f);
    void startFrame();
    void endEventLoop();
    void endPlayerInputProcessing();
    void endRandomTick();
    void endChunksUpdate();
    void endEntitiesProcessing();
    void endChunksRendering();
    void endEntitiesRendering();
    void endParticlesRendering();
    void endOverlaysRendering();
    void endFrame();
    void updateLabels();
};

}  // End namespace mc

#endif  // PERFDEBUGINFO_HPP