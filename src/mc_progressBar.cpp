#include <SFML/Graphics.hpp>

#include "mc_progressBar.hpp"

namespace mc {

void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &this->texture;
    target.draw(this->vertexArray, states);
}

ProgressBar::ProgressBar(sf::Texture& texture, bool isVertical) : texture(texture) {
    this->isVertical = isVertical;
    this->vertexArray.resize(6);
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->textureSize = sf::Vector2i(this->texture.getSize());
}

void ProgressBar::initializeVertexArray() {
    this->vertexArray[0].position = sf::Vector2f(0.f, 0.f);  // Top left
    this->vertexArray[1].position = sf::Vector2f(static_cast<float>(this->textureSize.x), 0.f);  // Top right
    this->vertexArray[2].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
    this->vertexArray[3].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
    this->vertexArray[4].position = sf::Vector2f(static_cast<float>(this->textureSize.x), 0.f);  // Top right
    this->vertexArray[5].position = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y));  // Bottom right

    this->vertexArray[0].texCoords = sf::Vector2f(0.f, 0.f);  // Top left
    this->vertexArray[1].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), 0.f);  // Top right
    this->vertexArray[2].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
    this->vertexArray[3].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
    this->vertexArray[4].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), 0.f);  // Top right
    this->vertexArray[5].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y));  // Bottom right
}

void ProgressBar::updateVertexArray() {
    if (this->isVertical) {
        this->vertexArray[0].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top left
        this->vertexArray[1].position = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top right
        this->vertexArray[2].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[3].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[4].position = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top right
        this->vertexArray[5].position = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y));  // Bottom right

        this->vertexArray[0].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top left
        this->vertexArray[1].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top right
        this->vertexArray[2].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[3].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[4].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y) * (1.f - this->progress));  // Top right
        this->vertexArray[5].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x), static_cast<float>(this->textureSize.y));  // Bottom right
    } else {
        this->vertexArray[0].position = sf::Vector2f(0.f, 0.f);  // Top left
        this->vertexArray[1].position = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, 0.f);  // Top right
        this->vertexArray[2].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[3].position = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[4].position = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, 0.f);  // Top right
        this->vertexArray[5].position = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, static_cast<float>(this->textureSize.y));  // Bottom right

        this->vertexArray[0].texCoords = sf::Vector2f(0.f, 0.f);  // Top left
        this->vertexArray[1].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, 0.f);  // Top right
        this->vertexArray[2].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[3].texCoords = sf::Vector2f(0.f, static_cast<float>(this->textureSize.y));  // Bottom left
        this->vertexArray[4].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, 0.f);  // Top right
        this->vertexArray[5].texCoords = sf::Vector2f(static_cast<float>(this->textureSize.x) * this->progress, static_cast<float>(this->textureSize.y));  // Bottom right
    }
}

void ProgressBar::setProgress(float progress) {
    this->progress = progress;
    this->updateVertexArray();
}

}  // namespace mc