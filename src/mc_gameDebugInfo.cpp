#include "mc_gameDebugInfo.hpp"

#include <SFML/Graphics.hpp>
#include <format>
#include <algorithm>

namespace mc {

void GameDebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->playerLocationLabel, states);
    target.draw(this->playerVelocityLabel, states);
    target.draw(this->mouseLocationLabel, states);
    target.draw(this->playerLightLevelLabel, states);
    target.draw(this->mouseLightLevelLabel, states);
    target.draw(this->loadedChunksLabel, states);
}

void GameDebugInfo::setPosition(sf::Vector2f position) {
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

void GameDebugInfo::setFillColor(sf::Color fillColor) {
    this->fillColor = fillColor;
    this->updateSettings();
}

void GameDebugInfo::setOutlineColor(sf::Color outlineColor) {
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
    this->playerVelocityLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->mouseLocationLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->playerLightLevelLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->mouseLightLevelLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->loadedChunksLabel.setPosition(calulatedPosition);
    this->playerLocationLabel.setFont(this->font);
    this->mouseLocationLabel.setFont(this->font);
    this->playerLightLevelLabel.setFont(this->font);
    this->mouseLightLevelLabel.setFont(this->font);
    this->loadedChunksLabel.setFont(this->font);
    this->playerVelocityLabel.setFont(this->font);
    this->playerLocationLabel.setCharacterSize(this->characterSize);
    this->mouseLocationLabel.setCharacterSize(this->characterSize);
    this->playerLightLevelLabel.setCharacterSize(this->characterSize);
    this->mouseLightLevelLabel.setCharacterSize(this->characterSize);
    this->loadedChunksLabel.setCharacterSize(this->characterSize);
    this->playerVelocityLabel.setCharacterSize(this->characterSize);
    this->playerLocationLabel.setFillColor(this->fillColor);
    this->mouseLocationLabel.setFillColor(this->fillColor);
    this->playerLightLevelLabel.setFillColor(this->fillColor);
    this->mouseLightLevelLabel.setFillColor(this->fillColor);
    this->loadedChunksLabel.setFillColor(this->fillColor);
    this->playerVelocityLabel.setFillColor(this->fillColor);
    this->playerLocationLabel.setOutlineColor(this->outlineColor);
    this->mouseLocationLabel.setOutlineColor(this->outlineColor);
    this->playerLightLevelLabel.setOutlineColor(this->outlineColor);
    this->mouseLightLevelLabel.setOutlineColor(this->outlineColor);
    this->loadedChunksLabel.setOutlineColor(this->outlineColor);
    this->playerVelocityLabel.setOutlineColor(this->outlineColor);
    this->playerLocationLabel.setOutlineThickness(this->outlineThickness);
    this->mouseLocationLabel.setOutlineThickness(this->outlineThickness);
    this->playerLightLevelLabel.setOutlineThickness(this->outlineThickness);
    this->mouseLightLevelLabel.setOutlineThickness(this->outlineThickness);
    this->loadedChunksLabel.setOutlineThickness(this->outlineThickness);
    this->playerVelocityLabel.setOutlineThickness(this->outlineThickness);
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

void GameDebugInfo::setPlayerVelocity(sf::Vector2f velocity) {
    this->playerVelocity = velocity;
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

void GameDebugInfo::setLoadedChunks(sf::Vector2i loadedChunks) {
    this->loadedChunks = loadedChunks;
}

void GameDebugInfo::updateLabels() {
    this->playerLocationLabel.setString(std::format("Player location: {} {:06.3f} {:.3f}", this->playerChunkID, this->playerPos.x, this->playerPos.y));
    this->playerLocationLabel.setOrigin(this->playerLocationLabel.getLocalBounds().width, 0.f);
    this->playerVelocityLabel.setString(std::format("Player velocity: {:.3f} {:.3f}", this->playerVelocity.x, this->playerVelocity.y));
    this->playerVelocityLabel.setOrigin(this->playerVelocityLabel.getLocalBounds().width, 0.f);
    this->mouseLocationLabel.setString(std::format("Mouse location: {} {} {}", this->mouseChunkID, this->mousePos.x, this->mousePos.y));
    this->mouseLocationLabel.setOrigin(this->mouseLocationLabel.getLocalBounds().width, 0.f);
    this->playerLightLevelLabel.setString(std::format("Player light level: {} ({} sky, {} block)", std::max(this->playerLightLevels.x, this->playerLightLevels.y), this->playerLightLevels.x, this->playerLightLevels.y));
    this->playerLightLevelLabel.setOrigin(this->playerLightLevelLabel.getLocalBounds().width, 0.f);
    this->mouseLightLevelLabel.setString(std::format("Mouse light level: {} ({} sky, {} block)", std::max(this->mouseLightLevels.x, this->mouseLightLevels.y), this->mouseLightLevels.x, this->mouseLightLevels.y));
    this->mouseLightLevelLabel.setOrigin(this->mouseLightLevelLabel.getLocalBounds().width, 0.f);
    this->loadedChunksLabel.setString(std::format("Chunks loaded: [{}, {}] ({} total)", this->loadedChunks.x, this->loadedChunks.y, this->loadedChunks.y - this->loadedChunks.x + 1));
    this->loadedChunksLabel.setOrigin(this->loadedChunksLabel.getLocalBounds().width, 0.f);
}

}  // End namespace mc