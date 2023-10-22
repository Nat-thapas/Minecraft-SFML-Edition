#include <SFML/Graphics.hpp>
#include <iostream>

#include "mc_textBox.hpp"
 
namespace mc {

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->background, states);
    target.draw(this->text, states);
}

TextBox::TextBox(sf::Font& font, sf::Vector2f size, int scaling) : font(font) {
    this->size = size;
    this->background.setFillColor(sf::Color(0, 0, 0));
    this->background.setOutlineColor(sf::Color(160, 160, 160));
    this->text.setFont(this->font);
    this->text.setFillColor(sf::Color::White);
    this->text.setOutlineColor(sf::Color::Black);
    this->text.setLetterSpacing(1.25f);
    this->setScaling(scaling);
    this->setPosition(sf::Vector2f(0.f, 0.f));
}

void TextBox::updatePosition() {
    this->background.setPosition(this->position);
    sf::FloatRect textGlobalBound = this->text.getGlobalBounds();
    sf::Vector2i backgroundSize = sf::Vector2i(this->size) * this->scaling;
    sf::Vector2f newTextPosition;
    newTextPosition.x = (static_cast<float>(backgroundSize.x) - textGlobalBound.width) / 2.f + this->position.x;
    newTextPosition.y = (static_cast<float>(backgroundSize.y) - static_cast<float>(this->scaling * 12) * 1.375f) / 2.f + this->position.y;
    this->text.setPosition(newTextPosition);
}

void TextBox::setPosition(sf::Vector2f position) {
    this->position = position;
    this->updatePosition();
}

void TextBox::setScaling(int scaling) {
    if (this->scaling == scaling) {
        return;
    }
    this->scaling = scaling;
    this->background.setSize(sf::Vector2f(this->size.x * static_cast<float>(this->scaling) - static_cast<float>(this->scaling) * 2.f, this->size.y * static_cast<float>(this->scaling) - static_cast<float>(this->scaling) * 2.f));
    this->background.setOutlineThickness(this->scaling);
    this->background.setOrigin(sf::Vector2f(-this->scaling, -this->scaling));
    this->text.setCharacterSize(12 * this->scaling);
    this->text.setOutlineThickness(0.5f * this->scaling);
    this->updatePosition();
}

void TextBox::setDisplayString(sf::String string) {
    sf::String processedString = "";
    size_t stringSize = string.getSize();
    for (size_t i = 0; i + 1 < stringSize; i++) {
        if (string[i + 1] != '\b') {
            processedString += string[i];
        }
    }
    if (stringSize > 0) {
        size_t i = stringSize - 1;
        if (string[i] != '\b') {
            processedString += string[i];
        }
    }
    this->string = processedString;
    this->text.setString(this->string);
    this->updatePosition();
}

sf::String TextBox::getDisplayString() {
    return this->string;
}

void TextBox::setFocused(bool focused) {
    this->focused = focused;
    if (this->focused) {
        this->background.setOutlineColor(sf::Color(255, 255, 255));
    } else {
        this->background.setOutlineColor(sf::Color(160, 160, 160));
    }
}

bool TextBox::getFocused() {
    return this->focused;
}

sf::FloatRect TextBox::getGlobalBounds() {
    return this->background.getGlobalBounds();
}

sf::FloatRect TextBox::getLocalBounds() {
    return this->background.getLocalBounds();
}

}  // namespace mc