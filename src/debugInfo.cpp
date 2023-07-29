#include "debugInfo.hpp"

#include <SFML/Graphics.hpp>
#include <format>

namespace mc {

void DebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->fpsLabel, states);
    target.draw(this->eventLoopTimeLabel, states);
    target.draw(this->playerInputProcessingTimeLabel, states);
    target.draw(this->randomTickTimeLabel, states);
    target.draw(this->chucksUpdateTimeLabel, states);
    target.draw(this->entitiesProcessingTimeLabel, states);
    target.draw(this->chunksRenderingTimeLabel, states);
    target.draw(this->entitiesRenderingTimeLabel, states);
    target.draw(this->particlesRenderingTimeLabel, states);
    target.draw(this->overlaysRenderingTimeLabel, states);
}

void DebugInfo::setPosition(sf::Vector2f& position) {
    this->position = position;
    this->updateSettings();
}

void DebugInfo::setFont(sf::Font& font) {
    this->font = font;
    this->updateSettings();
}

void DebugInfo::setCharacterSize(unsigned characterSize) {
    this->characterSize = characterSize;
    this->updateSettings();
}

void DebugInfo::setFillColor(sf::Color& fillColor) {
    this->fillColor = fillColor;
    this->updateSettings();
}

void DebugInfo::setOutlineColor(sf::Color& outlineColor) {
    this->outlineColor = outlineColor;
    this->updateSettings();
}

void DebugInfo::setOutlineThickness(float outlineThickness) {
    this->outlineThickness = outlineThickness;
    this->updateSettings();
}

void DebugInfo::setLineHeightMultiplier(float lineHeightMultiplier) {
    this->lineHeightMultiplier = lineHeightMultiplier;
    this->updateSettings();
}

void DebugInfo::updateSettings() {
    sf::Vector2f calulatedPosition(this->position);
    this->fpsLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->eventLoopTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->playerInputProcessingTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->randomTickTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->chucksUpdateTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->entitiesProcessingTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->chunksRenderingTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->entitiesRenderingTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->particlesRenderingTimeLabel.setPosition(calulatedPosition);
    calulatedPosition.y += this->characterSize * this->lineHeightMultiplier;
    this->overlaysRenderingTimeLabel.setPosition(calulatedPosition);
    this->fpsLabel.setFont(this->font);
    this->eventLoopTimeLabel.setFont(this->font);
    this->playerInputProcessingTimeLabel.setFont(this->font);
    this->randomTickTimeLabel.setFont(this->font);
    this->chucksUpdateTimeLabel.setFont(this->font);
    this->entitiesProcessingTimeLabel.setFont(this->font);
    this->chunksRenderingTimeLabel.setFont(this->font);
    this->entitiesRenderingTimeLabel.setFont(this->font);
    this->particlesRenderingTimeLabel.setFont(this->font);
    this->overlaysRenderingTimeLabel.setFont(this->font);
    this->fpsLabel.setCharacterSize(this->characterSize);
    this->eventLoopTimeLabel.setCharacterSize(this->characterSize);
    this->playerInputProcessingTimeLabel.setCharacterSize(this->characterSize);
    this->randomTickTimeLabel.setCharacterSize(this->characterSize);
    this->chucksUpdateTimeLabel.setCharacterSize(this->characterSize);
    this->entitiesProcessingTimeLabel.setCharacterSize(this->characterSize);
    this->chunksRenderingTimeLabel.setCharacterSize(this->characterSize);
    this->entitiesRenderingTimeLabel.setCharacterSize(this->characterSize);
    this->particlesRenderingTimeLabel.setCharacterSize(this->characterSize);
    this->overlaysRenderingTimeLabel.setCharacterSize(this->characterSize);
    this->fpsLabel.setFillColor(this->fillColor);
    this->eventLoopTimeLabel.setFillColor(this->fillColor);
    this->playerInputProcessingTimeLabel.setFillColor(this->fillColor);
    this->randomTickTimeLabel.setFillColor(this->fillColor);
    this->chucksUpdateTimeLabel.setFillColor(this->fillColor);
    this->entitiesProcessingTimeLabel.setFillColor(this->fillColor);
    this->chunksRenderingTimeLabel.setFillColor(this->fillColor);
    this->entitiesRenderingTimeLabel.setFillColor(this->fillColor);
    this->particlesRenderingTimeLabel.setFillColor(this->fillColor);
    this->overlaysRenderingTimeLabel.setFillColor(this->fillColor);
    this->fpsLabel.setOutlineColor(this->outlineColor);
    this->eventLoopTimeLabel.setOutlineColor(this->outlineColor);
    this->playerInputProcessingTimeLabel.setOutlineColor(this->outlineColor);
    this->randomTickTimeLabel.setOutlineColor(this->outlineColor);
    this->chucksUpdateTimeLabel.setOutlineColor(this->outlineColor);
    this->entitiesProcessingTimeLabel.setOutlineColor(this->outlineColor);
    this->chunksRenderingTimeLabel.setOutlineColor(this->outlineColor);
    this->entitiesRenderingTimeLabel.setOutlineColor(this->outlineColor);
    this->particlesRenderingTimeLabel.setOutlineColor(this->outlineColor);
    this->overlaysRenderingTimeLabel.setOutlineColor(this->outlineColor);
    this->fpsLabel.setOutlineThickness(this->outlineThickness);
    this->eventLoopTimeLabel.setOutlineThickness(this->outlineThickness);
    this->playerInputProcessingTimeLabel.setOutlineThickness(this->outlineThickness);
    this->randomTickTimeLabel.setOutlineThickness(this->outlineThickness);
    this->chucksUpdateTimeLabel.setOutlineThickness(this->outlineThickness);
    this->entitiesProcessingTimeLabel.setOutlineThickness(this->outlineThickness);
    this->chunksRenderingTimeLabel.setOutlineThickness(this->outlineThickness);
    this->entitiesRenderingTimeLabel.setOutlineThickness(this->outlineThickness);
    this->particlesRenderingTimeLabel.setOutlineThickness(this->outlineThickness);
    this->overlaysRenderingTimeLabel.setOutlineThickness(this->outlineThickness);
}

DebugInfo::DebugInfo(sf::Vector2f position, sf::Font font, unsigned characterSize, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, float lineHeightMultiplier) {
    this->position = position;
    this->font = font;
    this->characterSize = characterSize;
    this->fillColor = fillColor;
    this->outlineColor = outlineColor;
    this->outlineThickness = outlineThickness;
    this->lineHeightMultiplier = lineHeightMultiplier;
    this->updateSettings();
    this->frameTime = 0;
    this->eventLoopTime = 0 ;
    this->playerInputProcessingTime = 0 ;
    this->randomTickTime = 0 ;
    this->chunksUpdateTime = 0 ;
    this->entitiesProcessingTime = 0 ;
    this->chunksRenderingTime = 0 ;
    this->entitiesRenderingTime = 0 ;
    this->particlesRenderingTime = 0 ;
    this->overlaysRenderingTime = 0 ;
}

void DebugInfo::startFrame() {
    this->clock.restart();
}

void DebugInfo::endEventLoop() {
    this->eventLoopTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endPlayerInputProcessing() {
    this->playerInputProcessingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endRandomTick() {
    this->randomTickTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endChunksUpdate() {
    this->chunksUpdateTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endEntitiesProcessing() {
    this->entitiesProcessingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endChunksRendering() {
    this->chunksRenderingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endEntitiesRendering() {
    this->entitiesRenderingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endParticlesRendering() {
    this->particlesRenderingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endOverlaysRendering() {
    this->overlaysRenderingTime = this->clock.restart().asMicroseconds();
}

void DebugInfo::endFrame() {
    this->frameTime = this->frameTimeClock.restart().asMicroseconds();
}

void DebugInfo::updateLabels() {
    this->fpsLabel.setString(std::format("FPS: {:.2f} ({:.2f} ms.)", 1000000.f / this->frameTime, this->frameTime/1000.f));
    this->eventLoopTimeLabel.setString(std::format("Event Loop: {:.2f} ms.", this->eventLoopTime / 1000.f));
    this->playerInputProcessingTimeLabel.setString(std::format("Input Processing: {:.2f}", this->playerInputProcessingTime / 1000.f));
    this->randomTickTimeLabel.setString(std::format("Random Tick: {:.2f}", this->randomTickTime / 1000.f));
    this->chucksUpdateTimeLabel.setString(std::format("Chunk Updates: {:.2f}", this->chunksUpdateTime / 1000.f));
    this->entitiesProcessingTimeLabel.setString(std::format("Entities Processing: {:.2f}", this->entitiesProcessingTime / 1000.f));
    this->chunksRenderingTimeLabel.setString(std::format("Chunks Rendering: {:.2f}", this->chunksRenderingTime / 1000.f));
    this->entitiesRenderingTimeLabel.setString(std::format("Entities Rendering: {:.2f}", this->entitiesRenderingTime / 1000.f));
    this->particlesRenderingTimeLabel.setString(std::format("Particles Rendering: {:.2f}", this->particlesRenderingTime / 1000.f));
    this->overlaysRenderingTimeLabel.setString(std::format("Overlays Rendering: {:.2f}", this->overlaysRenderingTime / 1000.f));
}

}  // End namespace mc