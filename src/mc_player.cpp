#include "mc_player.hpp"

#include <SFML/Graphics.hpp>
#include <string>

#include "mc_chunks.hpp"
#include "idiv.hpp"
#include "mod.hpp"

namespace mc {

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->sprite, states);
}

Player::Player(Chunks& chunks, int chunkID, sf::Vector2f position, sf::Vector2i screenSize, int pixelPerBlock, std::string textureFilePath, float movementForce, float mass, float gravity, float frictionCoefficient) : chunks(chunks) {
    this->chunkID = chunkID;
    this->position = position;
    this->screenSize = screenSize;
    this->pixelPerBlock = pixelPerBlock;
    this->texture.loadFromFile(textureFilePath);
    this->sprite.setTexture(this->texture);
    float spriteScaleX =  static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->texture.getSize().x);
    float spriteScaleY =  static_cast<float>(this->pixelPerBlock * 2) / static_cast<float>(this->texture.getSize().y);
    this->sprite.setScale(sf::Vector2f(spriteScaleX, spriteScaleY));
    this->sprite.setOrigin(sf::Vector2f(static_cast<float>(this->pixelPerBlock) / 2.f, static_cast<float>(this->pixelPerBlock)));
    this->sprite.setPosition(sf::Vector2f(static_cast<float>(this->screenSize.x) / 2.f, static_cast<float>(this->screenSize.y) / 2.f));
    this->movementForce = movementForce;
    this->mass = mass;
    this->gravity = gravity;
    this->frictionCoefficient = frictionCoefficient;
}

void Player::setScreenSize(sf::Vector2i screenSize) {
    this->screenSize = screenSize;
    this->sprite.setPosition(sf::Vector2f(static_cast<float>(this->screenSize.x) / 2.f, static_cast<float>(this->screenSize.y) / 2.f));
}

void Player::setPixelPerBlock(int pixelPerBlock) {
    this->pixelPerBlock = pixelPerBlock;
    float spriteScaleX =  static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->texture.getSize().x);
    float spriteScaleY =  static_cast<float>(this->pixelPerBlock * 2) / static_cast<float>(this->texture.getSize().y);
    this->sprite.setScale(sf::Vector2f(spriteScaleX, spriteScaleY));
    this->sprite.setOrigin(sf::Vector2f(static_cast<float>(this->pixelPerBlock) / 2.f, static_cast<float>(this->pixelPerBlock)));
}

int Player::getChunkID() {
    return this->chunkID;
}

sf::Vector2f Player::getPosition() {
    return this->position;
}

}  // namespace mc