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
    for (size_t i = 0; i < this->chunks.size(); i++) {
        target.draw(this->chunks[i], states);
    }
}

void Chunks::updateTexture() {
    std::ifstream atlasDataFile(this->atlasDataPath);
    this->atlasData = json::parse(atlasDataFile);
    this->textureAtlas.loadFromFile(this->atlasFilePath);
}

void Chunks::initializeChunks() {
    for (int i=0; i<this->chunkCountOnScreen; i++) {
        this->chunks.push_back(Chunk(this->sampleChunk, this->textureAtlas, this->atlasData));
    }
}

Chunks::Chunks(int playerChunkID, int pixelPerBlock, int screenWidth, std::string atlasFilePath, std::string atlasDataPath) {
    this->playerChunkID = playerChunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->screenWidth = screenWidth;
    this->atlasFilePath = atlasFilePath;
    this->atlasDataPath = atlasDataPath;
    this->chunksStartID = this->playerChunkID - std::ceil((this->screenWidth / 2.f) / (16.f * this->pixelPerBlock));
    this->chunksEndID = this->playerChunkID + std::ceil((this->screenWidth / 2.f) / (16.f * this->pixelPerBlock)); 
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    this->updateTexture();
    this->initializeChunks();
}

void Chunks::updateChunksPosition() {
    int chunkID;
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenWidth / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    for (size_t i = 0; i < this->chunks.size(); i++) {
        this->chunks[i].setPosition(sf::Vector2f(fChunkXPos + i * this->pixelPerBlock * 16.f, -1.f * this->playerPos.y * this->pixelPerBlock));
    }
}

void Chunks::tickAnimation() {
    for (size_t i = 0; i < this->chunks.size(); i++) {
        this->chunks[i].tickAnimation();
    }
}

int Chunks::getPlayerChunkID() {
    return this->playerChunkID;
}

void Chunks::setPlayerChunkID(int chunkID) {
    if (this->playerChunkID == chunkID) {
        return;
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