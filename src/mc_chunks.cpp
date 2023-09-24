#include "mc_chunks.hpp"

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "idiv.hpp"
#include "mc_chunk.hpp"
#include "mod.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

void Chunks::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const Chunk& chunk : this->chunks) {
        target.draw(chunk, states);
    }
    target.draw(highlighter, states);
}

void Chunks::updateTexture() {
    if (this->pixelPerBlock < 16) {
        std::ifstream atlasDataFile(this->atlasDatasPath + std::format("blocksAtlasx{}.json", this->pixelPerBlock));
        this->atlasData = json::parse(atlasDataFile);
        this->textureAtlas.loadFromFile(this->atlasFilesPath + std::format("blocksAtlasx{}.png", this->pixelPerBlock));
    } else {
        std::ifstream atlasDataFile(this->atlasDatasPath + "blocksAtlasx16.json");
        this->atlasData = json::parse(atlasDataFile);
        this->textureAtlas.loadFromFile(this->atlasFilesPath + "blocksAtlasx16.png");
    }
}

void Chunks::initializeChunks() {
    for (int i = 0; i < this->chunkCountOnScreen; i++) {
        if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", this->chunksStartID + i))) {
            this->chunks.push_back(Chunk(std::format("saves/test/chunks/{}.dat", this->chunksStartID + i), this->chunksStartID + i, this->pixelPerBlock, this->textureAtlas, this->atlasData));
        } else {
            this->chunks.push_back(Chunk(this->noise, this->chunksStartID + i, this->pixelPerBlock, this->textureAtlas, this->atlasData));
        }
    }
}

Chunks::Chunks(int playerChunkID, int seed, int pixelPerBlock, sf::Vector2i screenSize, std::string atlasFilesPath, std::string atlasDatasPath) {
    this->seed = seed;
    this->playerChunkID = playerChunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->screenSize = screenSize;
    this->atlasFilesPath = atlasFilesPath;
    this->atlasDatasPath = atlasDatasPath;
    this->chunksStartID = this->playerChunkID - std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunksEndID = this->playerChunkID + std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    Perlin noise(this->seed);
    this->noise = noise;
    this->highlighter.setSize(sf::Vector2f(static_cast<float>(pixelPerBlock - 2), static_cast<float>(pixelPerBlock - 2)));
    this->highlighter.setOrigin(-1.f, -1.f);
    this->highlighter.setFillColor(sf::Color(0, 0, 0, 0));
    this->highlighter.setOutlineThickness(2.f);
    this->highlighter.setOutlineColor(sf::Color::Black);
    this->updateTexture();
    this->initializeChunks();
}

bool Chunks::saveAll() {
    int i = chunksStartID;
    for (Chunk& chunk : this->chunks) {
        chunk.saveToFile(std::format("saves/test/chunks/{}.dat", i));
        i++;
    }
    return true;
}

void Chunks::updateChunksPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenSize.x / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenSize.y / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int i = 0;
    for (Chunk& chunk : this->chunks) {
        chunk.setPosition(sf::Vector2f(fChunkXPos + i * this->pixelPerBlock * 16.f, chunkYPos));
        i++;
    }
}

void Chunks::updateMousePosition() {
    sf::Vector2f distance(sf::Vector2f(this->mouseScreenPos) - sf::Vector2f(this->screenSize.x / 2.f, this->screenSize.y / 2.f + (float)this->pixelPerBlock));
    distance /= (float)this->pixelPerBlock;
    distance += playerPos;
    int chunkDistance = std::lround(distance.x / 16.f - 0.5f);
    distance.x = mod(distance.x, 16);
    this->mouseChunkID = this->playerChunkID + chunkDistance;
    this->mousePos.x = std::lround(distance.x - 0.5f);
    this->mousePos.y = std::lround(distance.y - 0.5f);
    this->updateHighlighterPosition();
}

void Chunks::updateHighlighterPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenSize.x / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenSize.y / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int i = this->mouseChunkID - this->chunksStartID;
    this->highlighter.setPosition(fChunkXPos + i * this->pixelPerBlock * 16.f + mousePos.x * this->pixelPerBlock, chunkYPos + (float)(mousePos.y * this->pixelPerBlock));
}

void Chunks::tick(int tickCount) {
    for (Chunk& chunk : this->chunks) {
        chunk.tick(tickCount);
    }
}

void Chunks::updateVertexArrays() {
    for (Chunk& chunk : this->chunks) {
        chunk.updateVertexArray();
    }
}

void Chunks::setPixelPerBlock(int pixelPerBlock) {
    if (this->pixelPerBlock == pixelPerBlock) {
        return;
    }
    this->pixelPerBlock = pixelPerBlock;
    this->updateTexture();
    if (this->pixelPerBlock > 2) {
        this->highlighter.setSize(sf::Vector2f(static_cast<float>(this->pixelPerBlock - 2), static_cast<float>(this->pixelPerBlock - 2)));
    } else {
        this->highlighter.setSize(sf::Vector2f(static_cast<float>(this->pixelPerBlock), static_cast<float>(this->pixelPerBlock)));
    }
    for (Chunk& chunk : this->chunks) {
        chunk.setPixelPerBlock(this->pixelPerBlock);
    }
    int oldChunksStartID = this->chunksStartID;
    int oldChunksEndID = this->chunksEndID;
    this->chunksStartID = this->playerChunkID - std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunksEndID = this->playerChunkID + std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    if (this->chunksEndID > oldChunksEndID) {
        for (int i = 0; i < this->chunksEndID - oldChunksEndID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", oldChunksEndID + i + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/test/chunks/{}.dat", oldChunksEndID + i + 1), oldChunksEndID + i + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_back(Chunk(this->noise, oldChunksEndID + i + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
        }
    } else if (this->chunksEndID < oldChunksEndID) {
        for (int i = 0; i < oldChunksEndID - this->chunksEndID; i++) {
            this->chunks.back().saveToFile(std::format("saves/test/chunks/{}.dat", oldChunksEndID - i));
            this->chunks.pop_back();
        }
    }
    if (this->chunksStartID < oldChunksStartID) {
        for (int i = 0; i < oldChunksStartID - this->chunksStartID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", oldChunksStartID - i - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/test/chunks/{}.dat", oldChunksStartID - i - 1), oldChunksStartID - i - 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_front(Chunk(this->noise, oldChunksStartID - i - 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
        }
    } else if (this->chunksStartID > oldChunksStartID) {
        for (int i = 0; i < this->chunksStartID - oldChunksStartID; i++) {
            this->chunks.front().saveToFile(std::format("saves/test/chunks/{}.dat", oldChunksStartID + i));
            this->chunks.pop_front();
        }
    }
    this->updateChunksPosition();
    this->updateMousePosition();
}

void Chunks::setScreenSize(sf::Vector2i screenSize) {
    if (this->screenSize == screenSize) {
        return;
    }
    this->screenSize = screenSize;
    int oldChunksStartID = this->chunksStartID;
    int oldChunksEndID = this->chunksEndID;
    this->chunksStartID = this->playerChunkID - std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunksEndID = this->playerChunkID + std::lround((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock) + 0.5f);
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    if (this->chunksEndID > oldChunksEndID) {
        for (int i = 0; i < this->chunksEndID - oldChunksEndID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", oldChunksEndID + i + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/test/chunks/{}.dat", oldChunksEndID + i + 1), oldChunksEndID + i + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_back(Chunk(this->noise, oldChunksEndID + i + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
        }
    } else if (this->chunksEndID < oldChunksEndID) {
        for (int i = 0; i < oldChunksEndID - this->chunksEndID; i++) {
            this->chunks.back().saveToFile(std::format("saves/test/chunks/{}.dat", oldChunksEndID - i));
            this->chunks.pop_back();
        }
    }
    if (this->chunksStartID < oldChunksStartID) {
        for (int i = 0; i < oldChunksStartID - this->chunksStartID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", oldChunksStartID - i - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/test/chunks/{}.dat", oldChunksStartID - i - 1), oldChunksStartID - i - 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_front(Chunk(this->noise, oldChunksStartID - i - 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
        }
    } else if (this->chunksStartID > oldChunksStartID) {
        for (int i = 0; i < this->chunksStartID - oldChunksStartID; i++) {
            this->chunks.front().saveToFile(std::format("saves/test/chunks/{}.dat", oldChunksStartID + i));
            this->chunks.pop_front();
        }
    }
    this->updateChunksPosition();
    this->updateMousePosition();
}

int Chunks::getPlayerChunkID() {
    return this->playerChunkID;
}

void Chunks::setPlayerChunkID(int chunkID) {
    if (this->playerChunkID == chunkID) {
        return;
    }
    if (chunkID > this->playerChunkID) {
        for (int i = 0; i < chunkID - this->playerChunkID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", this->chunksEndID + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/test/chunks/{}.dat", this->chunksEndID + 1), this->chunksEndID + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_back(Chunk(this->noise, this->chunksEndID + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
            this->chunks.front().saveToFile(std::format("saves/test/chunks/{}.dat", this->chunksStartID));
            this->chunks.pop_front();
            this->chunksStartID++;
            this->chunksEndID++;
        }
    } else {
        for (int i = 0; i < this->playerChunkID - chunkID; i++) {
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", this->chunksStartID - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/test/chunks/{}.dat", this->chunksStartID - 1), this->chunksStartID + 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_front(Chunk(this->noise, this->chunksStartID - 1, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
            this->chunks.back().saveToFile(std::format("saves/test/chunks/{}.dat", this->chunksEndID));
            this->chunks.pop_back();
            this->chunksStartID--;
            this->chunksEndID--;
        }
    }
    this->playerChunkID = chunkID;
    this->updateChunksPosition();
    this->updateMousePosition();
}

sf::Vector2f Chunks::getPlayerPos() {
    return this->playerPos;
}

void Chunks::setPlayerPos(sf::Vector2f pos) {
    if (this->playerPos == pos) {
        return;
    }
    this->playerPos = pos;
    this->updateChunksPosition();
    this->updateMousePosition();
}

void Chunks::setMouseScreenPos(sf::Vector2i pos) {
    if (this->mouseScreenPos == pos) {
        return;
    }
    this->mouseScreenPos = pos;
    this->updateMousePosition();
}

int Chunks::getMouseChunkID() {
    return this->mouseChunkID;
}

sf::Vector2i Chunks::getMousePos() {
    return this->mousePos;
}

sf::Vector2i Chunks::getLoadedChunks() {
    return sf::Vector2i(this->chunksStartID, this->chunksEndID);
}

int Chunks::breakBlock(int& xp) {
    int breakChunkIndex = this->mouseChunkID - this->chunksStartID;
    return this->chunks[breakChunkIndex].breakBlock(mousePos.x, mousePos.y, xp);
}

}  // namespace mc