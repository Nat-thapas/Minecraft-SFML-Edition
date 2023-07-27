#include "debugInfo.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <format>
#include <iostream>

void DebugInfo::update() {
    fpsLabel.setString(std::format("FPS: {:.2f}", 1000000.f / frameTime));
    cpuTimeLabel.setString(std::format("CPU: {:.2f} ms.", cpuTime / 1000.f));
    gpuTimeLabel.setString(std::format("GPU: {:.2f} ms.", gpuTime / 1000.f));
}

void DebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(fpsLabel);
    target.draw(cpuTimeLabel);
    target.draw(gpuTimeLabel);
}

DebugInfo::DebugInfo(sf::Vector2f position, sf::Font font, unsigned int characterSize, float outlineThickness, sf::Color fillColor, sf::Color outlineColor) {
    this->position = position;
    this->font = font;
    this->characterSize = characterSize;
    this->outlineThickness = outlineThickness;
    this->fillColor = fillColor;
    this->outlineColor = outlineColor;
    this->frameTime = -1;
    this->cpuTime = -1;
    this->gpuTime = -1;
    this->fpsLabel.setPosition(this->position);
    this->position.y += round(this->characterSize * 1.25);
    this->cpuTimeLabel.setPosition(this->position);
    this->position.y += round(this->characterSize * 1.25);
    this->gpuTimeLabel.setPosition(this->position);
    this->fpsLabel.setFont(this->font);
    this->cpuTimeLabel.setFont(this->font);
    this->gpuTimeLabel.setFont(this->font);
    this->fpsLabel.setCharacterSize(this->characterSize);
    this->cpuTimeLabel.setCharacterSize(this->characterSize);
    this->gpuTimeLabel.setCharacterSize(this->characterSize);
    this->fpsLabel.setFillColor(this->fillColor);
    this->cpuTimeLabel.setFillColor(this->fillColor);
    this->gpuTimeLabel.setFillColor(this->fillColor);
    this->fpsLabel.setOutlineThickness(this->outlineThickness);
    this->cpuTimeLabel.setOutlineThickness(this->outlineThickness);
    this->gpuTimeLabel.setOutlineThickness(this->outlineThickness);
    this->fpsLabel.setOutlineColor(this->outlineColor);
    this->cpuTimeLabel.setOutlineColor(this->outlineColor);
    this->gpuTimeLabel.setOutlineColor(this->outlineColor);
}

void DebugInfo::startCpuTime() {
    cpuTimeClock.restart();
}

void DebugInfo::endCpuTime() {
    cpuTime = cpuTimeClock.restart().asMicroseconds();
}

void DebugInfo::startGpuTime() {
    gpuTimeClock.restart();
}

void DebugInfo::endGpuTime() {
    gpuTime = gpuTimeClock.restart().asMicroseconds();
}

void DebugInfo::endFrame() {
    frameTime = frameTimeClock.restart().asMicroseconds();
    update();
}