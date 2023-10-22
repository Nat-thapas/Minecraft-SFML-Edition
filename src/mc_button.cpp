#include <SFML/Graphics.hpp>

#include "mc_button.hpp"

namespace mc {

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->sprite, states);
    target.draw(this->text, states);
}

Button::Button(sf::Texture& texture, sf::Font& font, int scaling, std::string displayText) : texture(texture), font(font) {
    this->sprite.setTexture(this->texture);
    this->text.setFont(this->font);
    this->text.setString(displayText);
    this->text.setFillColor(sf::Color::White);
    this->text.setOutlineColor(sf::Color::Black);
    this->text.setLetterSpacing(1.25f);
    this->setState(BTN_STATE_NORMAL);
    this->setScaling(scaling);
    this->setPosition(sf::Vector2f(0.f, 0.f));
}

void Button::updatePosition() {
    this->sprite.setPosition(this->position);
    sf::FloatRect textGlobalBound = this->text.getGlobalBounds();
    sf::Vector2i buttonSize = sf::Vector2i(this->texture.getSize()) * this->scaling;
    buttonSize.y /= 3;
    sf::Vector2f newTextPosition;
    newTextPosition.x = (static_cast<float>(buttonSize.x) - textGlobalBound.width) / 2.f + this->position.x;
    newTextPosition.y = (static_cast<float>(buttonSize.y) - static_cast<float>(this->scaling * 12) * 1.375f) / 2.f + this->position.y;
    this->text.setPosition(newTextPosition);
}

void Button::setScaling(int scaling) {
    this->scaling = scaling;
    this->sprite.setScale(sf::Vector2f(this->scaling, this->scaling));
    this->text.setCharacterSize(12 * this->scaling);
    this->text.setOutlineThickness(0.5f * this->scaling);
    this->updatePosition();
}

void Button::setPosition(sf::Vector2f position) {
    this->position = position;
    this->updatePosition();
}

void Button::setDisplayText(std::string displayText) {
    this->text.setString(displayText);
    this->updatePosition();
}
void Button::setState(int state) {
    int textureWidth = this->texture.getSize().x;
    int textureHeight = this->texture.getSize().y / 3;
    this->sprite.setTextureRect(sf::IntRect(0, textureHeight * (state + 1), textureWidth, textureHeight));
}

sf::FloatRect Button::getGlobalBounds() {
    return this->sprite.getGlobalBounds();
}

sf::FloatRect Button::getLocalBounds() {
    return this->sprite.getLocalBounds();
}

}  // namespace mc;