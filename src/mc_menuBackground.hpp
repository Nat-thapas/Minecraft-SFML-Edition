#ifndef MC_MENUBACKGROUND_HPP
#define MC_MENUBACKGROUND_HPP

#include <SFML/Graphics.hpp>

namespace mc {

class MenuBackground : public sf::Drawable {
    sf::Sprite sprite1;
    sf::Sprite sprite2;
    sf::Texture& texture;
    float scale;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    MenuBackground(sf::Texture& texture);
    void setPosition(sf::Vector2f position);
    void setScale(float scale);
};

}  // namespace mc

#endif  // MC_MENUBACKGROUND_HPP