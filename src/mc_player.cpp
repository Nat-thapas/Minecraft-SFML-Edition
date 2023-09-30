#include "mc_player.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>

#include "mc_chunks.hpp"
#include "idiv.hpp"
#include "mod.hpp"

namespace mc {

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->sprite, states);
}

Player::Player(Chunks& chunks, int chunkID, sf::Vector2f position, sf::Vector2i screenSize, int pixelPerBlock, std::string textureFilePath, float movementForce, float mass, float gravity, float frictionCoefficient, float airDragCoefficient) : chunks(chunks) {
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
    this->acceleration.y = gravity;
    this->frictionCoefficient = frictionCoefficient;
    this->airDragCoefficient = airDragCoefficient;
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

bool Player::isOnGround() {
    return static_cast<float>(std::ceil(this->position.y)) - this->position.y < 0.0025f && this->isBlockSolid(this->getRelativeBlock(sf::Vector2f(0.f, 1.f)));
}

int Player::getRelativeBlock(sf::Vector2f delta) {
    sf::Vector2f newPosition(this->position + delta);
    int newChunkID = this->chunkID;
    newChunkID += static_cast<int>(std::floor(newPosition.x / 16.f));
    newPosition.x = mod(newPosition.x, 16.f);
    int xPos = static_cast<int>(std::floor(newPosition.x));
    int yPos = static_cast<int>(std::ceil(newPosition.y)) - 1;
    return this->chunks.getBlock(newChunkID, xPos, yPos);
}

int Player::getRelativeChunkBlock(sf::Vector2f position) {
    sf::Vector2f newPosition = position;
    int newChunkID = this->chunkID;
    newChunkID += static_cast<int>(std::floor(newPosition.x / 16.f));
    newPosition.x = mod(newPosition.x, 16.f);
    int xPos = static_cast<int>(std::floor(newPosition.x));
    int yPos = static_cast<int>(std::ceil(newPosition.y)) - 1;
    return this->chunks.getBlock(newChunkID, xPos, yPos);
}

int Player::getAbsoluteBlock(int chunkID, sf::Vector2f position) {
    sf::Vector2f newPosition = position;
    int newChunkID = chunkID;
    newChunkID += static_cast<int>(std::floor(newPosition.x / 16.f));
    newPosition.x = mod(newPosition.x, 16.f);
    int xPos = static_cast<int>(std::floor(newPosition.x));
    int yPos = static_cast<int>(std::ceil(newPosition.y)) - 1;
    return this->chunks.getBlock(newChunkID, xPos, yPos);
}

bool Player::isBlockSolid(int blockID) {
    return this->blockSolidity[blockID];
}

void Player::setLateralForce(int force) {
    force = std::clamp(force, -1, 1);
    this->acceleration.x = (force * this->movementForce) - (this->velocity.x * this->frictionCoefficient);
}

void Player::jump() {
    if (this->isOnGround()) {
        this->velocity.y -= static_cast<float>(sqrt(2.f * this->gravity * 1.25f));  // v^2 = 2as, target jump height = 1.25 m.
    }
}

void Player::update(sf::Time frameTime) {
    float deltaTime = std::min(frameTime.asSeconds(), 16.6666666666666666667f);  // Limit the physics rate to be above 60 Hz even if the game slows down
    this->velocity += this->acceleration * deltaTime;
    sf::Vector2f newPosition(this->position + this->velocity * deltaTime);
    // Check if player is sinking in to a block
    if (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x, newPosition.y)))) {
        newPosition.y = std::floor(newPosition.y);
        this->velocity.y = std::min(this->velocity.y, 0.f);
    }
    // Check if player head is rising in to a block
    if (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x, newPosition.y - 1.8f)))) {
        newPosition.y = std::floor(newPosition.y) + 0.8f;
        this->velocity.y = std::max(this->velocity.y, 0.f);
    }
    // Check if player body is going left in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.3f, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.3f, newPosition.y - 1.f))))) {
        newPosition.x = std::floor(newPosition.x) + 0.3f;
        this->velocity.x = std::max(this->velocity.x, 0.f);
    }
    // Check if player body is going right in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.3f, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.3f, newPosition.y - 1.f))))) {
        newPosition.x = std::floor(newPosition.x) + 0.7f;
        this->velocity.x = std::max(this->velocity.x, 0.f);
    }
    int newChunkID = this->chunkID;
    newChunkID += static_cast<int>(std::floor(newPosition.x / 16.f));
    newPosition.x = mod(newPosition.x, 16.f);
    this->chunkID = newChunkID;
    this->position = newPosition;
}

}  // namespace mc