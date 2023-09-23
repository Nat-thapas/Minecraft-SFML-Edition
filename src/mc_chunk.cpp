#include "mc_chunk.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &this->textureAtlas;
    target.draw(this->vertexArray, states);
}

Chunk::Chunk(int blocks[4096], int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData) : textureAtlas(textureAtlas), atlasData(atlasData) {
    this->chunkID = chunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    for (int i = 0; i < 4096; i++) {
        this->blocks[i] = blocks[i];
    }
    this->animationIndex = 0;
    this->initializeVertexArray();
    this->updateAllVertexArray();
}

Chunk::Chunk(std::string filePath, int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData) : textureAtlas(textureAtlas), atlasData(atlasData) {
    this->chunkID = chunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    std::ifstream inFile(filePath, std::ios::binary);
    inFile.read(reinterpret_cast<char*>(this->blocks), sizeof(this->blocks));
    inFile.close();
    this->animationIndex = 0;
    this->initializeVertexArray();
    this->updateAllVertexArray();
}

Chunk::Chunk(Perlin& noise, int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData) : textureAtlas(textureAtlas), atlasData(atlasData) {
    this->chunkID = chunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    int plantType;
    srand(this->chunkID);
    for (int i = 0; i < 4096; i++) {
        this->blocks[i] = 0;
    }
    for (int x = 0; x < 16; x++) {
        int height = static_cast<int>(noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 100.0, 8, 0.4) * 100 + 136);
        for (int y = 0; y < 256; y++) {
            if (y > 191) {
                this->blocks[x + y * 16] = 11;
            }
            if (y > height) {
                if (y - height <= 1) {
                    if (height > 191) {
                        if (height > 200) {
                            this->blocks[x + y * 16] = 19;
                        } else {
                            this->blocks[x + y * 16] = 15;
                        }
                    } else {
                        this->blocks[x + y * 16] = 2;
                        plantType = rand() % 20;
                        if (x > 1 && x < 14 && plantType < 3) {
                            for (int ox = 0; ox < 5; ox++) {
                                for (int oy = 0; oy < 7; oy++) {
                                    if (this->oakTrees[0][ox + oy * 5] && this->blocks[(x + ox - 2) + (y + oy - 7) * 16] != 6) {
                                        this->blocks[(x + ox - 2) + (y + oy - 7) * 16] = this->oakTrees[0][ox + oy * 5];
                                    }
                                }
                            }
                        } else if (plantType < 10) {
                            this->blocks[x + (y - 1) * 16] = 9;
                        } else if (plantType < 12) {
                            this->blocks[x + (y - 1) * 16] = 28;
                        } else if (plantType < 14) {
                            this->blocks[x + (y - 1) * 16] = 29;
                        }
                    }
                } else if (y - height <= 5) {
                    if (height > 191) {
                        if (height > 200) {
                            this->blocks[x + y * 16] = 19;
                        } else {
                            this->blocks[x + y * 16] = 15;
                        }
                    } else {
                        this->blocks[x + y * 16] = 3;
                    }
                } else if (y < 255 - rand() % 5) {
                    if (abs(y - (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 100.0 - 8572688.0, 4, 0.4) * 250.0 + 100.0)) < (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 50.0 + 3599341.0, 4, 0.4) * 6.0) || abs(y - (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 100.0 + 6238173.0, 4, 0.4) * 250.0 + 100.0)) < (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 50.0 - 4800281.0, 4, 0.4) * 6.0)) {
                        this->blocks[x + y * 16] = 0;
                    } else {
                        this->blocks[x + y * 16] = 1;
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 10.0, static_cast<double>(y) / 10.0 + 3925672.0, 4, 0.4) > 0.65) {
                            this->blocks[x + y * 16] = 20;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 - 6027822.0, 4, 0.4) > 0.675) {
                            this->blocks[x + y * 16] = 21;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 + 7734628.0, 4, 0.4) > 0.725 && y > 224) {
                            this->blocks[x + y * 16] = 22;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 - 1858459.0, 4, 0.4) > 0.725 && y > 232) {
                            this->blocks[x + y * 16] = 23;
                        }
                    }
                } else {
                    this->blocks[x + y * 16] = 10;
                }
            }
        }
    }
    this->animationIndex = 0;
    this->initializeVertexArray();
    this->updateAllVertexArray();
}

void Chunk::initializeVertexArray() {
    sf::IntRect blockRect;

    blockRect.width = this->pixelPerBlock;
    blockRect.height = this->pixelPerBlock;

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
        textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
        textureRect.height = textureRect.width;
        if (blockID == 13) {
            animationLength = idiv(static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]), textureRect.height);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * textureRect.height;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
            textureRect.left += mod(i, 2) * textureRect.width;
        } else {
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
        textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
        textureRect.height = textureRect.width;
        if (blockID == 13) {
            animationLength = idiv(static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]), textureRect.height);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * textureRect.height;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
            textureRect.left += mod(i, 2) * textureRect.width;
        } else {
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
        textureRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
        textureRect.height = textureRect.width;
        if (blockID == 13) {
            animationLength = idiv(static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]), textureRect.height);
            textureAnimationIndex = mod((this->animationIndex), animationLength * 2 - 1);
            if (textureAnimationIndex >= animationLength) {
                textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
            }
            textureRect.top += textureAnimationIndex * textureRect.height;
        } else if (blockID == 11 || blockID == 63) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
        } else if (blockID == 12 || blockID == 14 || blockID == 38) {
            textureRect.top += mod((textureRect.height * this->animationIndex), static_cast<int>(this->atlasData[std::format("{:03d}", blockID)]["h"]));
            textureRect.left += mod(i, 2) * textureRect.width;
        } else {
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

void Chunk::setPixelPerBlock(int pixelPerBlock) {
    this->pixelPerBlock = pixelPerBlock;
    this->initializeVertexArray();
    this->updateAllVertexArray();
}

int Chunk::getBlock(int x, int y) {
    return this->blocks[x + y * 16];
}

void Chunk::setBlock(int x, int y, int blockID) {
    this->blocks[x + y * 16] = blockID;
    this->blockUpdateQueue.push(x + y * 16);
    this->update();
    this->vertexUpdateQueue.push(x + y * 16);
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

int Chunk::breakBlock(int x, int y, int& xp) {
    int blockID = this->getBlock(x, y);
    this->setBlock(x, y, 0);
    xp += this->experienceDropAmount[blockID];
    int dropIntensity = rand() % 100 + 1;
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

void Chunk::tick(int tickCount) {
    this->animationIndex = tickCount;
    this->updateAnimatedVertexArray();
}

bool Chunk::saveToFile(std::string filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    outFile.write(reinterpret_cast<char*>(this->blocks), sizeof(this->blocks));
    outFile.close();
    return true;
}

}  // namespace mc