#ifndef MC_TEXTBOX_HPP
#define MC_TEXTBOX_HPP

#include <SFML/Graphics.hpp>

namespace mc {

class TextBox : public sf::Drawable {
    sf::RectangleShape background;
    sf::Text text;
    sf::Font& font;
    sf::String string;
    sf::Vector2f size;
    sf::Vector2f position;
    int scaling;
    bool focused;

    void updatePosition();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    TextBox(sf::Font& font, sf::Vector2f size, int scaling);
    void setPosition(sf::Vector2f position);
    void setScaling(int scaling);
    void setDisplayString(sf::String string);
    sf::String getDisplayString();
    void setFocused(bool focused);
    bool getFocused();
    sf::FloatRect getGlobalBounds();
    sf::FloatRect getLocalBounds();
};

}  // namespace mc

#endif  // MC_TEXTBOX_HPP