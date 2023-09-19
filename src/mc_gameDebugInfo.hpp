#ifndef MC_GAMEDEBUGINFO_HPP
#define MC_GAMEDEBUGINFO_HPP

#include <SFML/Graphics.hpp>
#include <format>
#include <algorithm>

namespace mc {

class GameDebugInfo : public sf::Drawable {
    sf::Font font;
    sf::Vector2f position;
    unsigned characterSize;
    sf::Color fillColor;
    sf::Color outlineColor;
    float outlineThickness;
    float lineHeightMultiplier;
    int playerChunkID;
    sf::Vector2f playerPos;
    int mouseChunkID;
    sf::Vector2i mousePos;
    sf::Vector2i playerLightLevels;
    sf::Vector2i mouseLightLevels;
    sf::Text playerLocationLabel;
    sf::Text mouseLocationLabel;
    sf::Text playerLightLevelLabel;
    sf::Text mouseLightLevelLabel;

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
    GameDebugInfo(sf::Vector2f position, sf::Font font, unsigned characterSize, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, float lineHeightMultiplier = 1.2f);
    void setPlayerChunkID(int chunkID);
    void setPlayerPos(sf::Vector2f pos);
    void setMouseChunkID(int chunkID);
    void setMousePos(sf::Vector2i pos);
    void setPlayerLightLevel(sf::Vector2i lightLevels);
    void setMouseLightLevel(sf::Vector2i lightLevels);
    void updateLabels();
};

}  // namespace mc

#endif  // MC_GAMEDEBUGINFO_HPP