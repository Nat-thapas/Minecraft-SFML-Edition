#include "mc_PerfDebugInfo.hpp"

#include <SFML/Graphics.hpp>
#include <format>

namespace mc {

void PerfDebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
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

void PerfDebugInfo::setPosition(sf::Vector2f& position) {
    this->position = position;
    this->updateSettings();
}

void PerfDebugInfo::setFont(sf::Font& font) {
    this->font = font;
    this->updateSettings();
}

void PerfDebugInfo::setCharacterSize(unsigned characterSize) {
    this->characterSize = characterSize;
    this->updateSettings();
}

void PerfDebugInfo::setFillColor(sf::Color& fillColor) {
    this->fillColor = fillColor;
    this->updateSettings();
}

void PerfDebugInfo::setOutlineColor(sf::Color& outlineColor) {
    this->outlineColor = outlineColor;
    this->updateSettings();
}

void PerfDebugInfo::setOutlineThickness(float outlineThickness) {
    this->outlineThickness = outlineThickness;
    this->updateSettings();
}

void PerfDebugInfo::setLineHeightMultiplier(float lineHeightMultiplier) {
    this->lineHeightMultiplier = lineHeightMultiplier;
    this->updateSettings();
}

void PerfDebugInfo::updateSettings() {
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

PerfDebugInfo::PerfDebugInfo(sf::Vector2f position, sf::Font font, unsigned characterSize, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, float lineHeightMultiplier) {
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

void PerfDebugInfo::startFrame() {
    this->clock.restart();
}

void PerfDebugInfo::endEventLoop() {
    this->eventLoopTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endPlayerInputProcessing() {
    this->playerInputProcessingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endRandomTick() {
    this->randomTickTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endChunksUpdate() {
    this->chunksUpdateTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endEntitiesProcessing() {
    this->entitiesProcessingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endChunksRendering() {
    this->chunksRenderingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endEntitiesRendering() {
    this->entitiesRenderingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endParticlesRendering() {
    this->particlesRenderingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endOverlaysRendering() {
    this->overlaysRenderingTime = this->clock.restart().asMicroseconds();
}

void PerfDebugInfo::endFrame() {
    this->frameTime = this->frameTimeClock.restart().asMicroseconds();
}

void PerfDebugInfo::updateLabels() {
    this->fpsLabel.setString(std::format("FPS: {:.2f} ({:.2f} ms.)", 1000000.f / this->frameTime, this->frameTime/1000.f));
    this->eventLoopTimeLabel.setString(std::format("Event Loop: {:.2f} ms.", this->eventLoopTime / 1000.f));
    this->playerInputProcessingTimeLabel.setString(std::format("Input Processing: {:.2f} ms.", this->playerInputProcessingTime / 1000.f));
    this->randomTickTimeLabel.setString(std::format("Random Tick: {:.2f} ms.", this->randomTickTime / 1000.f));
    this->chucksUpdateTimeLabel.setString(std::format("Chunk Updates: {:.2f} ms.", this->chunksUpdateTime / 1000.f));
    this->entitiesProcessingTimeLabel.setString(std::format("Entities Processing: {:.2f} ms.", this->entitiesProcessingTime / 1000.f));
    this->chunksRenderingTimeLabel.setString(std::format("Chunks Rendering: {:.2f} ms.", this->chunksRenderingTime / 1000.f));
    this->entitiesRenderingTimeLabel.setString(std::format("Entities Rendering: {:.2f} ms.", this->entitiesRenderingTime / 1000.f));
    this->particlesRenderingTimeLabel.setString(std::format("Particles Rendering: {:.2f} ms.", this->particlesRenderingTime / 1000.f));
    this->overlaysRenderingTimeLabel.setString(std::format("Overlays Rendering: {:.2f} ms.", this->overlaysRenderingTime / 1000.f));
}

}  // End namespace mc