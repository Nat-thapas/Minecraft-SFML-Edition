#include <SFML/Graphics.hpp>

#include "mc_menuBackground.hpp"

namespace mc {

void MenuBackground::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->sprite1, states);
    target.draw(this->sprite2, states);
}

MenuBackground::MenuBackground(sf::Texture& texture) : texture(texture) {
    this->sprite1.setTexture(this->texture);
    this->sprite2.setTexture(this->texture);
    this->sprite1.setColor(sf::Color(224, 224, 224));
    this->sprite2.setColor(sf::Color(224, 224, 224));
}

void MenuBackground::setPosition(sf::Vector2f position) {
    this->sprite1.setPosition(position);
    position.x += static_cast<float>(this->texture.getSize().x) * this->scale;
    this->sprite2.setPosition(position);
}

void MenuBackground::setScale(float scale) {
    this->scale = scale;
    this->sprite1.setScale(sf::Vector2f(scale, scale));
    this->sprite2.setScale(sf::Vector2f(scale, scale));

}

}  // namespace mc