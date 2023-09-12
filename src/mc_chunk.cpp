#include "mc_chunk.hpp"

#include <cstdlib>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>

#include "../include/json.hpp"
#include "mod.hpp"
#include "idiv.hpp"

using json = nlohmann::json;

namespace mc {

void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &this->textureAtlas;
    target.draw(this->vertexArray, states);
}

Chunk::Chunk(int blocks[4096], std::string textureAtlasFileName, std::string atlasDataFileName) {
    std::ifstream atlasDataFile(atlasDataFileName);
    this->atlasData = json::parse(atlasDataFile);
    this->textureAtlas.loadFromFile(textureAtlasFileName);
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    for (int i = 0; i < 4096; i++) {
        this->blocks[i] = blocks[i];
    }
    this->animationIndex = 0;
    this->initializeVertexArray();
    this->updateAllVertexArray();
}

void Chunk::initializeVertexArray() {
    sf::IntRect blockRect;

    blockRect.width = 64;
    blockRect.height = 64;

    for (int i = 0; i < 4096; i++) {
        blockRect.left = mod(i, 16) * blockRect.width;
        blockRect.top = idiv(i, 16) * blockRect.height;
        
        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex* triangles = &this->vertexArray[i * 6];

        // define the 6 corners of the two triangles
        triangles[0].position = sf::Vector2f(blockRect.left, blockRect.top);
        triangles[1].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top);
        triangles[2].position = sf::Vector2f(blockRect.left, blockRect.top + blockRect.height);
        triangles[3].position = sf::Vector2f(blockRect.left, blockRect.top + blockRect.height);
        triangles[4].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top);
        triangles[5].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top + blockRect.height);
    }
}

void Chunk::updateAnimatedVertexArray() {
    int blockID;
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;

    for (int i = 0; i < 4096; i++) {
        blockID = this->blocks[i];
        if (!(11 <= blockID && blockID <= 14) && blockID != 38 && blockID != 63) {
            continue;
        }
        textureRect.left = this->atlasData[std::format("{:03d}", blockID)]["x"];
        textureRect.top = this->atlasData[std::format("{:03d}", blockID)]["y"];
        if (blockID == 13) {
            textureRect.width = 16;
            textureRect.height = 16;
            animationLength = idiv((int)this->atlasData[std::format("{:03d}", blockID)]["h"], 16);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * 16;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
            textureRect.left += mod(i, 2) * 16;
        } else {
            textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
            textureRect.height = this->atlasData[std::format("{:03d}", blockID)]["h"];
        }

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex* triangles = &this->vertexArray[i * 6];

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        triangles[1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::updateAllVertexArray() {
    int blockID;
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;

    for (int i = 0; i < 4096; i++) {
        blockID = this->blocks[i];
        textureRect.left = this->atlasData[std::format("{:03d}", blockID)]["x"];
        textureRect.top = this->atlasData[std::format("{:03d}", blockID)]["y"];
        if (blockID == 13) {
            textureRect.width = 16;
            textureRect.height = 16;
            animationLength = idiv((int)this->atlasData[std::format("{:03d}", blockID)]["h"], 16);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * 16;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
            textureRect.left += mod(i, 2) * 16;
        } else {
            textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
            textureRect.height = this->atlasData[std::format("{:03d}", blockID)]["h"];
        }

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex* triangles = &this->vertexArray[i * 6];

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        triangles[1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::updateVertexArray() {
    int blockID;
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;
    int i;

    while (this->vertexUpdateQueue.size()) {
        i = vertexUpdateQueue.front();
        vertexUpdateQueue.pop();
        blockID = this->blocks[i];

        textureRect.left = this->atlasData[std::format("{:03d}", blockID)]["x"];
        textureRect.top = this->atlasData[std::format("{:03d}", blockID)]["y"];
        if (blockID == 13) {
            textureRect.width = 16;
            textureRect.height = 16;
            animationLength = idiv((int)this->atlasData[std::format("{:03d}", blockID)]["h"], 16);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * 16;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.width = 16;
            textureRect.height = 16;
            textureRect.top += mod((16 * this->animationIndex), (int)this->atlasData[std::format("{:03d}", blockID)]["h"]);
            textureRect.left += mod(i, 2) * 16;
        } else {
            textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
            textureRect.height = this->atlasData[std::format("{:03d}", blockID)]["h"];
        }

        // get a pointer to the triangles' vertices of the current tile
        sf::Vertex* triangles = &this->vertexArray[i * 6];

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        triangles[1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::update() {
    int i, blockID;
    while (this->blockUpdateQueue.size()) {
        i = this->blockUpdateQueue.front();
        this->blockUpdateQueue.pop();
        blockID = this->blocks[i];
        // TODO
    }
}

int Chunk::getBlock(int x, int y) {
    return this->blocks[x + y * 16];
}

void Chunk::setBlock(int x, int y, int blockID) {
    this->blocks[x + y * 16] = blockID;
    this->blockUpdateQueue.push(x + y * 16);
    this->update();
    this->vertexUpdateQueue.push(x + y * 16);
    this->updateVertexArray();
}

bool Chunk::placeBlock(int x, int y, int itemID) {
    int blockID = this->blockPlaceIDs[itemID];
    if (blockID == 0) {
        return false;
    }
    if (this->getBlock(x, y)) {
        return false;
    }
    this->setBlock(x, y, blockID);
    return true;
}

int Chunk::breakBlock(int x, int y, int *xp) {
    int blockID = this->getBlock(x, y);
    this->setBlock(x, y, 0);
    *xp += this->experienceDropAmount[blockID];
    int dropIntensity = rand()%100 + 1;
    if (blockID == 19 && dropIntensity > 90) {
        return 80;
    }
    int requiredDropIntensity = 100 - this->itemDropChances[blockID];
    if (dropIntensity > requiredDropIntensity) {
        return this->itemDropIDs[blockID];
    } else {
        return 0;
    }
}

void Chunk::tickAnimation() {
    this->animationIndex++;
    this->updateAnimatedVertexArray();
}

}  // namespace mc