#include "mc_chunks.hpp"

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;

namespace mc {

void Chunks::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const Chunk& chunk : this->chunks) {
        target.draw(chunk, states);
    }
}

void Chunks::updateTexture() {
    std::ifstream atlasDataFile(this->atlasDataPath);
    this->atlasData = json::parse(atlasDataFile);
    this->textureAtlas.loadFromFile(this->atlasFilePath);
}

void Chunks::initializeChunks() {
    for (int i = 0; i < this->chunkCountOnScreen; i++) {
        this->chunks.push_back(Chunk(this->sampleChunk, this->pixelPerBlock, this->textureAtlas, this->atlasData));
    }
}

Chunks::Chunks(int playerChunkID, int pixelPerBlock, int screenWidth, int screenHeight, std::string atlasFilePath, std::string atlasDataPath) {
    this->playerChunkID = playerChunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->atlasFilePath = atlasFilePath;
    this->atlasDataPath = atlasDataPath;
    this->chunksStartID = this->playerChunkID - std::ceil((this->screenWidth / 2.f) / (16.f * this->pixelPerBlock));
    this->chunksEndID = this->playerChunkID + std::ceil((this->screenWidth / 2.f) / (16.f * this->pixelPerBlock));
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    this->updateTexture();
    this->initializeChunks();
}

void Chunks::updateChunksPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenWidth / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenHeight / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int i = 0;
    for (Chunk& chunk : this->chunks) {
        chunk.setPosition(sf::Vector2f(fChunkXPos + i * this->pixelPerBlock * 16.f, chunkYPos));
        i++;
    }
}

void Chunks::tickAnimation() {
    for (Chunk& chunk : this->chunks) {
        chunk.tickAnimation();
    }
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
            this->chunks.push_back(Chunk(this->sampleChunk, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            this->chunks.pop_front();
            this->chunksStartID++;
            this->chunksEndID++;
        }
    } else {
        for (int i = 0; i < this->playerChunkID - chunkID; i++) {
            this->chunks.push_front(Chunk(this->sampleChunk, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            this->chunks.pop_back();
            this->chunksStartID--;
            this->chunksEndID--;
        }
    }
    this->playerChunkID = chunkID;
    this->updateChunksPosition();
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
}

}  // namespace mc