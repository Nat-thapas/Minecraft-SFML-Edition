#include "mc_chunk.hpp"

#include <format>
#include <fstream>
#include <string>
#include <vector>

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
    this->updateVertexArray(false);
}

void Chunk::updateVertexArray(bool onlyUpdateAnimated) {
    int blockID;
    sf::IntRect blockRect;
    sf::IntRect textureRect;

    blockRect.width = 64;
    blockRect.height = 64;

    int textureAnimationIndex;
    int animationLength;

    for (int i = 0; i < 4096; i++) {
        blockID = blocks[i];
        if (onlyUpdateAnimated && !(11 <= blockID && blockID <= 14) && blockID != 38 && blockID != 63) {
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

        // define the 6 matching texture coordinates
        triangles[0].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        triangles[1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        triangles[4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        triangles[5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

int Chunk::getBlock(int x, int y) {
    return this->blocks[x + y * 16];
}

void Chunk::setBlock(int x, int y, int blockID) {
    this->blocks[x + y * 16] = blockID;
    this->updateVertexArray(false);
}

void Chunk::tickAnimation() {
    this->animationIndex++;
}

}  // namespace mc