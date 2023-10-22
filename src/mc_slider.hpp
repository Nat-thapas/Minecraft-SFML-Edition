#ifndef MC_SLIDER_HPP
#define MC_SLIDER_HPP

#define SLIDER_STATE_NORMAL 0
#define SLIDER_STATE_HOVERED 1

#include <SFML/Graphics.hpp>

namespace mc {

class Slider : public sf::Drawable {
    sf::Sprite backgroundSprite;
    sf::Texture& backgroundTexture;
    sf::Sprite slidingSprite;
    sf::Texture& slidingTexture;
    sf::Text text;
    sf::Font& font;
    sf::Vector2f position;
    int scaling;
    int maxVal;
    int currentVal;
    bool active;

    void updatePosition();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Slider(sf::Texture& backgroundTexture, sf::Texture& slidingTexture, sf::Font& font, int scaling, std::string displayText, int maxVal);
    void setPosition(sf::Vector2f position);
    void setScaling(int scaling);
    void setDisplayText(std::string displayText);
    void setState(int state);
    sf::FloatRect getGlobalBounds();
    sf::FloatRect getLocalBounds();
    void setValue(int val);
    void setValueByMousePos(sf::Vector2i mousePos);
    int getValue();
    void setActiveValue(bool val);
    bool getActiveValue();
};

}  // namespace mc

#endif  // MC_SLIDER_HPP