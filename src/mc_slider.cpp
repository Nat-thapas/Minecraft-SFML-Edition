#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

#include "mc_slider.hpp"

namespace mc {

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->backgroundSprite, states);
    target.draw(this->slidingSprite, states);
    target.draw(this->text, states);
}

Slider::Slider(sf::Texture& backgroundTexture, sf::Texture& slidingTexture, sf::Font& font, int scaling, std::string displayText, int maxVal) : backgroundTexture(backgroundTexture), slidingTexture(slidingTexture), font(font) {
    this->backgroundSprite.setTexture(this->backgroundTexture);
    this->slidingSprite.setTexture(this->slidingTexture);
    this->text.setFont(this->font);
    this->text.setString(displayText);
    this->text.setFillColor(sf::Color::White);
    this->text.setOutlineColor(sf::Color::Black);
    this->text.setLetterSpacing(1.25f);
    this->currentVal = 0;
    this->maxVal = maxVal;
    this->setState(SLIDER_STATE_NORMAL);
    this->setScaling(scaling);
    this->setPosition(sf::Vector2f(0.f, 0.f));
}

void Slider::updatePosition() {
    this->backgroundSprite.setPosition(this->position);
    sf::FloatRect textGlobalBound = this->text.getGlobalBounds();
    sf::Vector2i sliderSize = sf::Vector2i(this->backgroundTexture.getSize()) * this->scaling;
    sf::Vector2f newTextPosition;
    newTextPosition.x = (static_cast<float>(sliderSize.x) - textGlobalBound.width) / 2.f + this->position.x;
    newTextPosition.y = (static_cast<float>(sliderSize.y) - static_cast<float>(this->scaling * 12) * 1.375f) / 2.f + this->position.y;
    this->text.setPosition(newTextPosition);
    sf::Vector2i slidingSize = sf::Vector2i(this->slidingTexture.getSize()) * this->scaling;
    slidingSize.x /= 2;
    int possibleSlidingPosition = sliderSize.x - slidingSize.x;
    sf::Vector2f newSlidingPosition;
    newSlidingPosition.y = this->position.y;
    newSlidingPosition.x = this->position.x + (static_cast<float>(this->currentVal) / static_cast<float>(this->maxVal)) * static_cast<float>(possibleSlidingPosition);
    this->slidingSprite.setPosition(newSlidingPosition);
}

void Slider::setPosition(sf::Vector2f position) {
    this->position = position;
    this->updatePosition();
}

void Slider::setScaling(int scaling) {
    if (this->scaling == scaling) {
        return;
    }
    this->scaling = scaling;
    this->backgroundSprite.setScale(sf::Vector2f(this->scaling, this->scaling));
    this->slidingSprite.setScale(sf::Vector2f(this->scaling, this->scaling));
    this->text.setCharacterSize(12 * this->scaling);
    this->text.setOutlineThickness(0.5f * this->scaling);
    this->updatePosition();
}

void Slider::setDisplayText(std::string displayText) {
    this->text.setString(displayText);
    this->updatePosition();
}

void Slider::setState(int state) {
    sf::Vector2i textureSize = sf::Vector2i(this->slidingTexture.getSize());
    textureSize.x /= 2;
    this->slidingSprite.setTextureRect(sf::IntRect(state * textureSize.x, 0, textureSize.x, textureSize.y));
}

sf::FloatRect Slider::getGlobalBounds() {
    return this->backgroundSprite.getGlobalBounds();
}

sf::FloatRect Slider::getLocalBounds() {
    return this->backgroundSprite.getLocalBounds();
}

void Slider::setValue(int val) {
    this->currentVal = val;
    this->updatePosition();
}

void Slider::setValueByMousePos(sf::Vector2i mousePos) {
    float slidingSize = this->slidingTexture.getSize().x / 2.f;
    float relatetiveMousePos = static_cast<float>(mousePos.x) - this->backgroundSprite.getPosition().x - slidingSize;
    float normalizedPosition = relatetiveMousePos / (this->backgroundSprite.getGlobalBounds().width - 2.f * slidingSize);
    normalizedPosition = std::clamp(normalizedPosition, 0.f, 1.f);
    this->currentVal = static_cast<int>(std::round(normalizedPosition * this->maxVal));
    this->updatePosition();
}

int Slider::getValue() {
    return this->currentVal;
}

void Slider::setActiveValue(bool val) {
    this->active = val;
}

bool Slider::getActiveValue() {
    return this->active;
}

}  // namespace mc