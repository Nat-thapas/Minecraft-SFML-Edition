#ifndef MC_BUTTON_HPP
#define MC_BUTTON_HPP

#define BTN_STATE_DISABLED -1
#define BTN_STATE_NORMAL 0
#define BTN_STATE_HOVERED 1

#include <SFML/Graphics.hpp>
#include <string>

namespace mc {

class Button : public sf::Drawable {
    sf::Sprite sprite;
    sf::Texture& texture;
    sf::Text text;
    sf::Font& font;
    sf::Vector2f position;
    int scaling;

    void updatePosition();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Button(sf::Texture& texture, sf::Font& font, int scaling, std::string displayText);
    void setPosition(sf::Vector2f position);
    void setScaling(int scaling);
    void setDisplayText(std::string displayText);
    void setState(int state);
    sf::FloatRect getGlobalBounds();
    sf::FloatRect getLocalBounds();
};

}  // namespace mc

#endif  // MC_BUTTON_HPP