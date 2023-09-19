#include "mc_gameDebugInfo.hpp"

#include <SFML/Graphics.hpp>
#include <format>
#include <algorithm>

namespace mc {

void GameDebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->playerLocationLabel, states);
    target.draw(this->mouseLocationLabel, states);
    target.draw(this->playerLightLevelLabel, states);
    target.draw(this->mouseLightLevelLabel, states);
}

void GameDebugInfo::setPosition(sf::Vector2f& position) {
    this->position = position;
    this->updateSettings();
}

void GameDebugInfo::setFont(sf::Font& font) {
    this->font = font;
    this->updateSettings();
}

void GameDebugInfo::setCharacterSize(unsigned characterSize) {
    this->characterSize = characterSize;
    this->updateSettings();
}

void GameDebugInfo::setFillColor(sf::Color& fillColor) {
    this->fillColor = fillColor;
    this->updateSettings();
}

void GameDebugInfo::setOutlineColor(sf::Color& outlineColor) {
    this->outlineColor = outlineColor;
    this->updateSettings();
}

void GameDebugInfo::setOutlineThickness(float outlineThickness) {
    this->outlineThickness = outlineThickness;
    this->updateSettings();
}

void GameDebugInfo::setLineHeightMultiplier(float lineHeightMultiplier) {
    this->lineHeightMultiplier = lineHeightMultiplier;
    this->updateSettings();
}

void GameDebugInfo::updateSettings() {
    sf::Vector2f calulatedPosition(this->position);
    this->playerLocationLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->mouseLocationLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->playerLightLevelLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->mouseLightLevelLabel.setPosition(calulatedPosition);
    this->playerLocationLabel.setFont(this->font);
    this->mouseLocationLabel.setFont(this->font);
    this->playerLightLevelLabel.setFont(this->font);
    this->mouseLightLevelLabel.setFont(this->font);
    this->playerLocationLabel.setCharacterSize(this->characterSize);
    this->mouseLocationLabel.setCharacterSize(this->characterSize);
    this->playerLightLevelLabel.setCharacterSize(this->characterSize);
    this->mouseLightLevelLabel.setCharacterSize(this->characterSize);
    this->playerLocationLabel.setFillColor(this->fillColor);
    this->mouseLocationLabel.setFillColor(this->fillColor);
    this->playerLightLevelLabel.setFillColor(this->fillColor);
    this->mouseLightLevelLabel.setFillColor(this->fillColor);
    this->playerLocationLabel.setOutlineColor(this->outlineColor);
    this->mouseLocationLabel.setOutlineColor(this->outlineColor);
    this->playerLightLevelLabel.setOutlineColor(this->outlineColor);
    this->mouseLightLevelLabel.setOutlineColor(this->outlineColor);
    this->playerLocationLabel.setOutlineThickness(this->outlineThickness);
    this->mouseLocationLabel.setOutlineThickness(this->outlineThickness);
    this->playerLightLevelLabel.setOutlineThickness(this->outlineThickness);
    this->mouseLightLevelLabel.setOutlineThickness(this->outlineThickness);
}

GameDebugInfo::GameDebugInfo(sf::Vector2f position, sf::Font font, unsigned characterSize, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, float lineHeightMultiplier) {
    this->position = position;
    this->font = font;
    this->characterSize = characterSize;
    this->fillColor = fillColor;
    this->outlineColor = outlineColor;
    this->outlineThickness = outlineThickness;
    this->lineHeightMultiplier = lineHeightMultiplier;
    this->updateSettings();
}

void GameDebugInfo::setPlayerChunkID(int chunkID) {
    this->playerChunkID = chunkID;
}

void GameDebugInfo::setPlayerPos(sf::Vector2f pos) {
    this->playerPos = pos;
}

void GameDebugInfo::setMouseChunkID(int chunkID) {
    this->mouseChunkID = chunkID;
}

void GameDebugInfo::setMousePos(sf::Vector2i pos) {
    this->mousePos = pos;
}

void GameDebugInfo::setPlayerLightLevel(sf::Vector2i lightLevels) {
    this->playerLightLevels = lightLevels;
}

void GameDebugInfo::setMouseLightLevel(sf::Vector2i lightLevels) {
    this->mouseLightLevels = lightLevels;
}

void GameDebugInfo::updateLabels() {
    this->playerLocationLabel.setString(std::format("Player location: {} {:06.3f} {:06.3f}", this->playerChunkID, this->playerPos.x, this->playerPos.y));
    this->mouseLocationLabel.setString(std::format("Mouse location: {} {} {}", this->mouseChunkID, this->mousePos.x, this->mousePos.y));
    this->playerLightLevelLabel.setString(std::format("Player light level: {} ({} sky, {} block)", std::max(this->playerLightLevels.x, this->playerLightLevels.y), this->playerLightLevels.x, this->playerLightLevels.y));
    this->mouseLightLevelLabel.setString(std::format("Mouse light level: {} ({} sky, {} block)", std::max(this->mouseLightLevels.x, this->mouseLightLevels.y), this->mouseLightLevels.x, this->mouseLightLevels.y));
}

}  // End namespace mc