#include <SFML/Graphics.hpp>
#include <algorithm>
#include <string>
#include <unordered_map>

#include "idiv.hpp"
#include "mod.hpp"

#include "mc_chunks.hpp"
#include "mc_player.hpp"

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
    float spriteScaleX =  static_cast<float>(this->pixelPerBlock) / 2.f / static_cast<float>(this->texture.getSize().x);
    float spriteScaleY =  static_cast<float>(this->pixelPerBlock) * 1.8f / static_cast<float>(this->texture.getSize().y);
    this->sprite.setScale(sf::Vector2f(spriteScaleX, spriteScaleY));
    this->sprite.setOrigin(sf::Vector2f(4.f, 16.f));
    this->sprite.setPosition(sf::Vector2f(static_cast<float>(this->screenSize.x) / 2.f, static_cast<float>(this->screenSize.y) / 2.f + static_cast<float>(this->pixelPerBlock) * 0.1f));
    this->movementForce = movementForce;
    this->mass = mass;
    this->gravity = gravity;
    this->acceleration.y = gravity;
    this->frictionCoefficient = frictionCoefficient;
    this->airDragCoefficient = airDragCoefficient;
}

void Player::setScreenSize(sf::Vector2i screenSize) {
    this->screenSize = screenSize;
    this->sprite.setPosition(sf::Vector2f(static_cast<float>(this->screenSize.x) / 2.f, static_cast<float>(this->screenSize.y) / 2.f + static_cast<float>(this->pixelPerBlock) * 0.1f));
}

void Player::setPixelPerBlock(int pixelPerBlock) {
    if (this->pixelPerBlock == pixelPerBlock) {
        return;
    }
    this->pixelPerBlock = pixelPerBlock;
    float spriteScaleX =  static_cast<float>(this->pixelPerBlock) / 2.f / static_cast<float>(this->texture.getSize().x);
    float spriteScaleY =  static_cast<float>(this->pixelPerBlock) * 1.8f / static_cast<float>(this->texture.getSize().y);
    this->sprite.setScale(sf::Vector2f(spriteScaleX, spriteScaleY));
    this->sprite.setPosition(sf::Vector2f(static_cast<float>(this->screenSize.x) / 2.f, static_cast<float>(this->screenSize.y) / 2.f + static_cast<float>(this->pixelPerBlock) * 0.1f));
}

int Player::getChunkID() {
    return this->chunkID;
}

sf::Vector2f Player::getPosition() {
    return this->position;
}

sf::Vector2f Player::getVelocity() {
    return this->velocity;
}

bool Player::isOnGround() {
    return static_cast<float>(std::ceil(this->position.y)) - this->position.y < 0.0025f && ((this->isBlockSolid(this->getRelativeBlock(sf::Vector2f(0.f, 1.f)))) || (this->isBlockSolid(this->getRelativeBlock(sf::Vector2f(-0.2975f, 1.f)))) || (this->isBlockSolid(this->getRelativeBlock(sf::Vector2f(0.2975f, 1.f)))));
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
    return this->blocksSolidity[blockID];
}

void Player::setLateralForce(int force, bool sprint) {
    force = std::clamp(force, -1, 1);
    if (this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 12) {
        if (force) {
            this->acceleration.x = ((force * this->movementForce * (1.f + 0.3f * sprint)) - (this->velocity.x * this->frictionCoefficient * 2.5f)) / this->mass;
        } else {
            this->acceleration.x = - (this->velocity.x * this->frictionCoefficient * 2.f) / this->mass;
        }
        if (!this->isOnGround()) {
            this->acceleration.x *= 0.25f;
        }
    } else {
        if (force) {
            this->acceleration.x = ((force * this->movementForce * (1.f + 0.3f * sprint)) - (this->velocity.x * this->frictionCoefficient)) / this->mass;
        } else {
            this->acceleration.x = - (this->velocity.x * this->frictionCoefficient * 2.f) / this->mass;
        }
        if (!this->isOnGround()) {
            this->acceleration.x *= 0.25f;
        }
    }
}

void Player::jump() {
    if (this->isOnGround()) {
        this->velocity.y -= static_cast<float>(sqrt(2.f * this->gravity * 1.25f));  // v^2 = 2as, target jump height = 1.25 m.
    } else if (this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 12) {
        if (this->velocity.y > 0) {
            this->velocity.y -= static_cast<float>(sqrt(2.f * this->gravity * 1.25f)) / 2.f;
        }
    }
}

void Player::physicsUpdate(float deltaTime) {
    if (this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(-0.3f, 0.f)) == 12 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 11 || this->getRelativeBlock(sf::Vector2f(0.3f, 0.f)) == 12) {
        this->acceleration.y = ((this->mass * this->gravity) - (this->velocity.y * this->airDragCoefficient * 25.f)) / this->mass;
    } else {
        this->acceleration.y = ((this->mass * this->gravity) - (this->velocity.y * this->airDragCoefficient)) / this->mass;
    }
    this->velocity += this->acceleration * deltaTime;
    sf::Vector2f newPosition = this->position;
    newPosition.y += this->velocity.y * deltaTime;
    // Check if player is sinking in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.2975f, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.2975f, newPosition.y))))) {
        newPosition.y = std::floor(newPosition.y);
        this->velocity.y = std::min(this->velocity.y, 0.f);
    }
    // Check if player head is rising in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x, newPosition.y - 1.8f)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.2975f, newPosition.y - 1.8f)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.2975f, newPosition.y - 1.8f))))) {
        newPosition.y = std::floor(newPosition.y) + 0.8f;
        this->velocity.y = std::max(this->velocity.y, 0.f);
    }
    newPosition.x += this->velocity.x * deltaTime;
    // Check if player body is going left in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.3f, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.3f, newPosition.y - 1.f)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x - 0.3f, newPosition.y - 1.7975f))))) {
        newPosition.x = std::floor(newPosition.x) + 0.3f;
        this->velocity.x = std::max(this->velocity.x, 0.f);
    }
    // Check if player body is going right in to a block
    if ((this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.3f, newPosition.y)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.3f, newPosition.y - 1.f)))) || (this->isBlockSolid(this->getRelativeChunkBlock(sf::Vector2f(newPosition.x + 0.3f, newPosition.y - 1.7975f))))) {
        newPosition.x = std::floor(newPosition.x) + 0.7f;
        this->velocity.x = std::min(this->velocity.x, 0.f);
    }
    int newChunkID = this->chunkID;
    newChunkID += static_cast<int>(std::floor(newPosition.x / 16.f));
    newPosition.x = mod(newPosition.x, 16.f);
    this->chunkID = newChunkID;
    this->position = newPosition;
}

void Player::update(sf::Time frameTime) {
    float deltaTime = frameTime.asSeconds();
    int stepCount = static_cast<int>(std::ceil(deltaTime / (1.f / this->targetPhysicsRate)));
    float stepDetaTime = deltaTime / static_cast<float>(stepCount);
    stepCount = std::min(stepCount, 100);
    for (int i = 0; i < stepCount; i++) {
        this->physicsUpdate(stepDetaTime);
    }
}

}  // namespace mc