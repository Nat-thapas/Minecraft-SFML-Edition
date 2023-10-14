#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <string>

#include "../include/perlin.hpp"
#include "idiv.hpp"
#include "mod.hpp"

#include "mc_chunk.hpp"

using Perlin = siv::PerlinNoise;

namespace mc {

void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(this->vertexArray, states);
}

Chunk::Chunk(int blocks[4096], int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData) : parsedAtlasData(parsedAtlasData) {
    this->chunkID = chunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    for (int i = 0; i < 4096; i++) {
        this->blocks[i] = blocks[i];
    }
    this->animationIndex = 0;
    this->initializeVertexArrays();
    this->updateAllVertexArray();
    this->initializeLightEngine();
    this->updateAllLightingVertexArray();
}

Chunk::Chunk(std::string filePath, int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData) : parsedAtlasData(parsedAtlasData) {
    this->chunkID = chunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(256 * 16 * 6);  // 256 blocks high, 16 blocks wide, 6 vertices per block
    std::ifstream inFile(filePath, std::ios::binary);
    inFile.read(reinterpret_cast<char*>(this->blocks.data()), this->blocks.size() * sizeof(int));
    inFile.close();
    this->animationIndex = 0;
    this->initializeVertexArrays();
    this->updateAllVertexArray();
    this->initializeLightEngine();
    this->updateAllLightingVertexArray();
}

Chunk::Chunk(Perlin& noise, int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData) : parsedAtlasData(parsedAtlasData) {
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
                        } else if (height > 196) {
                            this->blocks[x + y * 16] = 35;
                        } else {
                            this->blocks[x + y * 16] = 15;
                        }
                    } else {
                        this->blocks[x + y * 16] = 2;
                        plantType = rand() % 20;
                        if (x > 1 && x < 14 && plantType < 3) {
                            this->blocks[x + y * 16] = 3;
                            for (int ox = 0; ox < 5; ox++) {
                                for (int oy = 0; oy < 7; oy++) {
                                    if (this->oakTrees[plantType][ox + oy * 5] && this->blocks[(x + ox - 2) + (y + oy - 7) * 16] != 6) {
                                        this->blocks[(x + ox - 2) + (y + oy - 7) * 16] = this->oakTrees[plantType][ox + oy * 5];
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
                        } else if (height > 196) {
                            this->blocks[x + y * 16] = 35;
                        } else {
                            this->blocks[x + y * 16] = 15;
                        }
                    } else {
                        this->blocks[x + y * 16] = 3;
                    }
                } else if (y < 255 - rand() % 5) {
                    if (abs(y - (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 100.0 - 857.2688, 4, 0.4) * 250.0 + 100.0)) < (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 50.0 + 359.9341, 4, 0.4) * 6.0) || abs(y - (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 100.0 + 623.8173, 4, 0.4) * 250.0 + 100.0)) < (noise.normalizedOctave1D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 50.0 - 480.0281, 4, 0.4) * 6.0)) {
                        if (y > 248) {
                            this->blocks[x + y * 16] = 13;
                        } else {
                            this->blocks[x + y * 16] = 0;
                        }
                    } else {
                        this->blocks[x + y * 16] = 1;
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 10.0, static_cast<double>(y) / 10.0 - 743.1432, 4, 0.4) > 0.7) {
                            this->blocks[x + y * 16] = 19;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 10.0, static_cast<double>(y) / 10.0 + 392.5672, 4, 0.4) > 0.65) {
                            this->blocks[x + y * 16] = 20;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 - 602.7822, 4, 0.4) > 0.675) {
                            this->blocks[x + y * 16] = 21;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 + 773.4628, 4, 0.4) > 0.725 && y > 224) {
                            this->blocks[x + y * 16] = 22;
                        }
                        if (noise.normalizedOctave2D_01((this->chunkID * 16.0 + static_cast<double>(x)) / 5.0, static_cast<double>(y) / 5.0 - 185.8459, 4, 0.4) > 0.725 && y > 232) {
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
    this->initializeVertexArrays();
    this->updateAllVertexArray();
    this->initializeLightEngine();
    this->updateAllLightingVertexArray();
}

void Chunk::initializeVertexArrays() {
    sf::IntRect blockRect;

    blockRect.width = this->pixelPerBlock;
    blockRect.height = this->pixelPerBlock;

    for (int i = 0; i < 4096; i++) {
        blockRect.left = (i % 16) * blockRect.width;
        blockRect.top = (i / 16) * blockRect.height;

        this->vertexArray[i * 6].position = sf::Vector2f(blockRect.left, blockRect.top);
        this->vertexArray[i * 6 + 1].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top);
        this->vertexArray[i * 6 + 2].position = sf::Vector2f(blockRect.left, blockRect.top + blockRect.height);
        this->vertexArray[i * 6 + 3].position = sf::Vector2f(blockRect.left, blockRect.top + blockRect.height);
        this->vertexArray[i * 6 + 4].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top);
        this->vertexArray[i * 6 + 5].position = sf::Vector2f(blockRect.left + blockRect.width, blockRect.top + blockRect.height);
    }
}

void Chunk::updateAnimatedVertexArray() {
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;

    for (int i = 0; i < 4096; i++) {
        int blockID = this->blocks[i];
        if (!(11 <= blockID && blockID <= 14) && blockID != 38 && blockID != 63) {
            continue;
        }
        textureRect.left = this->parsedAtlasData[blockID].left;
        textureRect.top = this->parsedAtlasData[blockID].top;
        textureRect.width = this->parsedAtlasData[blockID].width;
        textureRect.height = textureRect.width;
        switch (blockID) {
            case 11:  // Still water
                textureRect.top += mod((textureRect.height * (this->animationIndex / 2)), this->parsedAtlasData[blockID].height);
                break;
            case 12:  // Flowing water
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 13:  // Still lava
                animationLength = idiv(this->parsedAtlasData[blockID].height, textureRect.height);
                textureAnimationIndex = mod(this->animationIndex / 2, animationLength * 2 - 1);
                if (textureAnimationIndex >= animationLength) {
                    textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
                }
                textureRect.top += textureAnimationIndex * textureRect.height;
                break;
            case 14:  // Flowing lava
                textureRect.top += mod((textureRect.height * (this->animationIndex / 3)), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 38:  // Fire
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 63:  // Nether portal
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                break;
            default:
                textureRect.height = this->parsedAtlasData[blockID].height;
                break;
        }

        this->vertexArray[i * 6].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        this->vertexArray[i * 6 + 1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::updateAllVertexArray() {
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;

    for (int i = 0; i < 4096; i++) {
        int blockID = this->blocks[i];
        textureRect.left = this->parsedAtlasData[blockID].left;
        textureRect.top = this->parsedAtlasData[blockID].top;
        textureRect.width = this->parsedAtlasData[blockID].width;
        textureRect.height = textureRect.width;
        switch (blockID) {
            case 11:  // Still water
                textureRect.top += mod((textureRect.height * (this->animationIndex / 2)), this->parsedAtlasData[blockID].height);
                break;
            case 12:  // Flowing water
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 13:  // Still lava
                animationLength = idiv(this->parsedAtlasData[blockID].height, textureRect.height);
                textureAnimationIndex = mod(this->animationIndex / 2, animationLength * 2 - 1);
                if (textureAnimationIndex >= animationLength) {
                    textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
                }
                textureRect.top += textureAnimationIndex * textureRect.height;
                break;
            case 14:  // Flowing lava
                textureRect.top += mod((textureRect.height * (this->animationIndex / 3)), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 38:  // Fire
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (i % 2) * textureRect.width;
                break;
            case 63:  // Nether portal
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                break;
            default:
                textureRect.height = this->parsedAtlasData[blockID].height;
                break;
        }

        this->vertexArray[i * 6].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        this->vertexArray[i * 6 + 1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::updateVertexArray() {
    sf::IntRect textureRect;

    int textureAnimationIndex;
    int animationLength;

    while (!this->vertexUpdateQueue.empty()) {
        int idx = this->vertexUpdateQueue.front();
        this->vertexUpdateQueue.pop();
        int blockID = this->blocks[idx];

        textureRect.left = this->parsedAtlasData[blockID].left;
        textureRect.top = this->parsedAtlasData[blockID].top;
        textureRect.width = this->parsedAtlasData[blockID].width;
        textureRect.height = textureRect.width;
        switch (blockID) {
            case 11:  // Still water
                textureRect.top += mod((textureRect.height * (this->animationIndex / 2)), this->parsedAtlasData[blockID].height);
                break;
            case 12:  // Flowing water
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (idx % 2) * textureRect.width;
                break;
            case 13:  // Still lava
                animationLength = idiv(this->parsedAtlasData[blockID].height, textureRect.height);
                textureAnimationIndex = mod(this->animationIndex / 2, animationLength * 2 - 1);
                if (textureAnimationIndex >= animationLength) {
                    textureAnimationIndex = animationLength * 2 - textureAnimationIndex - 1;
                }
                textureRect.top += textureAnimationIndex * textureRect.height;
                break;
            case 14:  // Flowing lava
                textureRect.top += mod((textureRect.height * (this->animationIndex / 3)), this->parsedAtlasData[blockID].height);
                textureRect.left += (idx % 2) * textureRect.width;
                break;
            case 38:  // Fire
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                textureRect.left += (idx % 2) * textureRect.width;
                break;
            case 63:  // Nether portal
                textureRect.top += mod((textureRect.height * this->animationIndex), this->parsedAtlasData[blockID].height);
                break;
            default:
                textureRect.height = this->parsedAtlasData[blockID].height;
                break;
        }

        this->vertexArray[idx * 6].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        this->vertexArray[idx * 6 + 1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[idx * 6 + 2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[idx * 6 + 3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[idx * 6 + 4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[idx * 6 + 5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Chunk::updateAllLightingVertexArray() {
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 16; x++) {
            this->vertexArray[(x + y * 16) * 6].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y - 1), this->getLightLevel(x, y - 1), this->getLightLevel(x - 1, y), this->getLightLevel(x, y)}));      // Top left
            this->vertexArray[(x + y * 16) * 6 + 1].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y - 1), this->getLightLevel(x + 1, y - 1), this->getLightLevel(x, y), this->getLightLevel(x + 1, y)}));  // Top right
            this->vertexArray[(x + y * 16) * 6 + 2].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y), this->getLightLevel(x, y), this->getLightLevel(x - 1, y + 1), this->getLightLevel(x, y + 1)}));  // Bottom left
            this->vertexArray[(x + y * 16) * 6 + 3].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y), this->getLightLevel(x, y), this->getLightLevel(x - 1, y + 1), this->getLightLevel(x, y + 1)}));  // Bottom left
            this->vertexArray[(x + y * 16) * 6 + 4].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y - 1), this->getLightLevel(x + 1, y - 1), this->getLightLevel(x, y), this->getLightLevel(x + 1, y)}));  // Top right
            this->vertexArray[(x + y * 16) * 6 + 5].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y), this->getLightLevel(x + 1, y), this->getLightLevel(x, y + 1), this->getLightLevel(x + 1, y + 1)}));  // Bottom right
        }
    }
}

void Chunk::updateLightingVertexArray() {
    while (!this->lightingVertexUpdateQueue.empty()) {
        int idx = this->lightingVertexUpdateQueue.front();
        this->lightingVertexUpdateQueue.pop();
        int X = idx % 16;
        int Y = idx / 16;
        for (int oX = -1; oX <= 1; oX++) {
            for (int oY = -1; oY <= 1; oY++) {
                int x = X + oX;
                int y = Y + oY;
                if (x < 0 || x > 15 || y < 0 || y > 255) {
                    continue;
                }
                this->vertexArray[(x + y * 16) * 6].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y - 1), this->getLightLevel(x, y - 1), this->getLightLevel(x - 1, y), this->getLightLevel(x, y)}));      // Top left
                this->vertexArray[(x + y * 16) * 6 + 1].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y - 1), this->getLightLevel(x + 1, y - 1), this->getLightLevel(x, y), this->getLightLevel(x + 1, y)}));  // Top right
                this->vertexArray[(x + y * 16) * 6 + 2].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y), this->getLightLevel(x, y), this->getLightLevel(x - 1, y + 1), this->getLightLevel(x, y + 1)}));  // Bottom left
                this->vertexArray[(x + y * 16) * 6 + 3].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x - 1, y), this->getLightLevel(x, y), this->getLightLevel(x - 1, y + 1), this->getLightLevel(x, y + 1)}));  // Bottom left
                this->vertexArray[(x + y * 16) * 6 + 4].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y - 1), this->getLightLevel(x + 1, y - 1), this->getLightLevel(x, y), this->getLightLevel(x + 1, y)}));  // Top right
                this->vertexArray[(x + y * 16) * 6 + 5].color = this->getColorFromLightLevel(std::max({this->getLightLevel(x, y), this->getLightLevel(x + 1, y), this->getLightLevel(x, y + 1), this->getLightLevel(x + 1, y + 1)}));  // Bottom right
            }
        }
    }
}

bool Chunk::isBlockOpaque(int x, int y) {
    return this->blockOpaqueness[this->getBlock(x, y)];
}

int Chunk::getBlockEmissionLevel(int x, int y) {
    return this->blockEmissionLevels[this->getBlock(x, y)];
}

sf::Color Chunk::getColorFromLightLevel(int lightLevel) {
    int val = lightLevel * (lightLevel + 1) + 15;
    return sf::Color(val, val, val, 255);
}

void Chunk::initializeLightEngine() {
    for (int i = 0; i < 4096; i++) {
        this->skyLightLevels[i] = 0;
        this->blockLightLevels[i] = 0;
    }

    for (int x = 0; x < 16; x++) {
        if (this->isBlockOpaque(x, 0)) {
            this->skyLightLevels[x + 0 * 16] = 0;
        } else {
            this->skyLightLevels[x + 0 * 16] = 15;
        }
        this->skyLightUpdateQueue.push(x + 1 * 16);
        this->lightingVertexUpdateQueue.push(x + 0 * 16);
    }

    for (int y = 0; y < 255; y++) {
        for (int x = 0; x < 16; x++) {
            int blockEmissionLevel = this->getBlockEmissionLevel(x, y);
            if (blockEmissionLevel > 0) {
                this->blockLightLevels[x + y * 16] = blockEmissionLevel;
                if (x > 0) {
                    this->blockLightUpdateQueue.push((x - 1) + y * 16);
                }
                if (x < 15) {
                    this->blockLightUpdateQueue.push((x + 1) + y * 16);
                }
                if (y > 0) {
                    this->blockLightUpdateQueue.push(x + (y - 1) * 16);
                }
                if (y < 255) {
                    this->blockLightUpdateQueue.push(x + (y + 1) * 16);
                }
                this->lightingVertexUpdateQueue.push(x + y * 16);
            }
        }
    }
}

void Chunk::update() {
    while (!this->blockUpdateQueue.empty()) {
        int idx = this->blockUpdateQueue.front();
        this->blockUpdateQueue.pop();
        int blockID = this->blocks[idx];
        int x = idx % 16;
        int y = idx / 16;
        this->vertexUpdateQueue.push(x + y * 16);
        this->skyLightUpdateQueue.push(x + y * 16);
        this->blockLightUpdateQueue.push(x + y * 16);
    }
}

void Chunk::updateLightLevels() {
    while (!this->skyLightUpdateQueue.empty()) {
        int idx = this->skyLightUpdateQueue.front();
        this->skyLightUpdateQueue.pop();
        int x = idx % 16;
        int y = idx / 16;
        int oldLightLevel = this->getSkyLightLevel(x, y);
        int lightLevel = 0;
        bool filterLevel = 11 <= this->getBlock(x, y) && this->getBlock(x, y) < 15;
        if (!this->isBlockOpaque(x, y)) {
            if (this->getSkyLightLevel(x, y - 1) - static_cast<int>(filterLevel) > lightLevel) {
                lightLevel = this->getSkyLightLevel(x, y - 1) - static_cast<int>(filterLevel);
            }
            if (this->getSkyLightLevel(x - 1, y) - 1 > lightLevel) {
                lightLevel = this->getSkyLightLevel(x - 1, y) - 1;
            }
            if (this->getSkyLightLevel(x + 1, y) - 1 > lightLevel) {
                lightLevel = this->getSkyLightLevel(x + 1, y) - 1;
            }
            if (this->getSkyLightLevel(x, y + 1) - 1 > lightLevel) {
                lightLevel = this->getSkyLightLevel(x, y + 1) - 1;
            }
            if (y == 0) {
                lightLevel = 15;
            } // TODO try delete this
        }
        if (oldLightLevel == lightLevel) {
            continue;
        }
        this->skyLightLevels[idx] = lightLevel;
        this->lightingVertexUpdateQueue.push(idx);
        if (x > 0) {
            this->skyLightUpdateQueue.push((x - 1) + y * 16);
        }
        if (x < 15) {
            this->skyLightUpdateQueue.push((x + 1) + y * 16);
        }
        if (y > 0) {
            this->skyLightUpdateQueue.push(x + (y - 1) * 16);
        }
        if (y < 255) {
            this->skyLightUpdateQueue.push(x + (y + 1) * 16);
        }
    }

    while (!this->blockLightUpdateQueue.empty()) {
        int idx = this->blockLightUpdateQueue.front();
        this->blockLightUpdateQueue.pop();
        int x = idx % 16;
        int y = idx / 16;
        int oldLightLevel = this->getBlockLightLevel(x, y);
        int lightLevel = 0;
        if (this->getBlockEmissionLevel(x, y)) {
            lightLevel = this->getBlockEmissionLevel(x, y);
        } else if (!this->isBlockOpaque(x, y)) {
            if (this->getBlockLightLevel(x, y - 1) - 1 > lightLevel) {
                lightLevel = this->getBlockLightLevel(x, y - 1) - 1;
            }
            if (this->getBlockLightLevel(x - 1, y) - 1 > lightLevel) {
                lightLevel = this->getBlockLightLevel(x - 1, y) - 1;
            }
            if (this->getBlockLightLevel(x + 1, y) - 1 > lightLevel) {
                lightLevel = this->getBlockLightLevel(x + 1, y) - 1;
            }
            if (this->getBlockLightLevel(x, y + 1) - 1 > lightLevel) {
                lightLevel = this->getBlockLightLevel(x, y + 1) - 1;
            }
        }
        if (oldLightLevel == lightLevel) {
            continue;
        }
        this->blockLightLevels[idx] = lightLevel;
        this->lightingVertexUpdateQueue.push(idx);
        if (x > 0) {
            this->blockLightUpdateQueue.push((x - 1) + y * 16);
        }
        if (x < 15) {
            this->blockLightUpdateQueue.push((x + 1) + y * 16);
        }
        if (y > 0) {
            this->blockLightUpdateQueue.push(x + (y - 1) * 16);
        }
        if (y < 255) {
            this->blockLightUpdateQueue.push(x + (y + 1) * 16);
        }
    }
}

void Chunk::setPixelPerBlock(int pixelPerBlock) {
    this->pixelPerBlock = pixelPerBlock;
    this->initializeVertexArrays();
    this->updateAllVertexArray();
}

int Chunk::getBlock(int x, int y) {
    if (x < 0 || x > 15 || y < 0 || y > 255) {
        return 0;
    }
    return this->blocks[x + y * 16];
}

void Chunk::setBlock(int x, int y, int blockID) {
    this->blocks[x + y * 16] = blockID;
    this->blockUpdateQueue.push(x + y * 16);
    this->update();
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
    // TODO random tick
}

int Chunk::getSkyLightLevel(int x, int y) {
    if (x >= 0 && x <= 15 && y == 256) {
        return 15;
    }
    if (x == -1 && y >= 0 && y <= 255) {
        return this->leftChunkSkyLightLevels[y];
    }
    if (x == 16 && y >= 0 && y <= 255) {
        return this->rightChunkSkyLightLevels[y];
    }
    if (x < 0 || x > 15 || y < 0 || y > 255) {
        return 0;
    }
    return this->skyLightLevels[x + y * 16];
}

int Chunk::getBlockLightLevel(int x, int y) {
    if (x == -1 && y >= 0 && y <= 255) {
        return this->leftChunkBlockLightLevels[y];
    }
    if (x == 16 && y >= 0 && y <= 255) {
        return this->rightChunkBlockLightLevels[y];
    }
    if (x < 0 || x > 15 || y < 0 || y > 255) {
        return 0;
    }
    return this->blockLightLevels[x + y * 16];
}

int Chunk::getLightLevel(int x, int y) {
    return std::max(this->getSkyLightLevel(x, y), this->getBlockLightLevel(x, y));
}

void Chunk::setLeftChunkSkyLightLevels(std::array<int, 256> lightLevels) {
    for (int y = 0; y < 256; y++) {
        if (this->leftChunkSkyLightLevels[y] != lightLevels[y]) {
            this->leftChunkSkyLightLevels[y] = lightLevels[y];
            this->skyLightUpdateQueue.push(0 + y * 16);
            this->lightingVertexUpdateQueue.push(0 + y * 16);
        }
    }
}

void Chunk::setRightChunkSkyLightLevels(std::array<int, 256> lightLevels) {
    for (int y = 0; y < 256; y++) {
        if (this->rightChunkSkyLightLevels[y] != lightLevels[y]) {
            this->rightChunkSkyLightLevels[y] = lightLevels[y];
            this->skyLightUpdateQueue.push(15 + y * 16);
            this->lightingVertexUpdateQueue.push(15 + y * 16);
        }
    }
}

void Chunk::setLeftChunkBlockLightLevels(std::array<int, 256> lightLevels) {
    for (int y = 0; y < 256; y++) {
        if (this->leftChunkBlockLightLevels[y] != lightLevels[y]) {
            this->leftChunkBlockLightLevels[y] = lightLevels[y];
            this->blockLightUpdateQueue.push(0 + y * 16);
            this->lightingVertexUpdateQueue.push(0 + y * 16);
        }
    }
}

void Chunk::setRightChunkBlockLightLevels(std::array<int, 256> lightLevels) {
    for (int y = 0; y < 256; y++) {
        if (this->rightChunkBlockLightLevels[y] != lightLevels[y]) {
            this->rightChunkBlockLightLevels[y] = lightLevels[y];
            this->blockLightUpdateQueue.push(15 + y * 16);
            this->lightingVertexUpdateQueue.push(15 + y * 16);
        }
    }
}

std::array<int, 256> Chunk::getLeftSkyLightLevels() {
    std::array<int, 256> output = {};
    for (int y = 0; y < 256; y++) {
        output[y] = this->getSkyLightLevel(0, y);
    }
    return output;
}

std::array<int, 256> Chunk::getRightSkyLightLevels() {
    std::array<int, 256> output = {};
    for (int y = 0; y < 256; y++) {
        output[y] = this->getSkyLightLevel(15, y);
    }
    return output;
}

std::array<int, 256> Chunk::getLeftBlockLightLevels() {
    std::array<int, 256> output = {};
    for (int y = 0; y < 256; y++) {
        output[y] = this->getBlockLightLevel(0, y);
    }
    return output;
}

std::array<int, 256> Chunk::getRightBlockLightLevels() {
    std::array<int, 256> output = {};
    for (int y = 0; y < 256; y++) {
        output[y] = this->getBlockLightLevel(15, y);
    }
    return output;
}

bool Chunk::saveToFile(std::string filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    outFile.write(reinterpret_cast<char*>(this->blocks.data()), this->blocks.size() * sizeof(int));
    outFile.close();
    return true;
}

}  // namespace mc