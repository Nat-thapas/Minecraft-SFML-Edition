#include "mc_chunks.hpp"

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
#include <filesystem>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;

namespace mc {

void Chunks::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const Chunk& chunk : this->chunks) {
        target.draw(chunk, states);
    }
    target.draw(highlighter, states);
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
    this->highlighter.setSize(sf::Vector2f((float)(pixelPerBlock - 2), (float)(pixelPerBlock - 2)));
    this->highlighter.setOrigin(-1.f, -1.f);
    this->highlighter.setFillColor(sf::Color(0, 0, 0, 0));
    this->highlighter.setOutlineThickness(2.f);
    this->highlighter.setOutlineColor(sf::Color::Black);
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

void Chunks::updateMousePosition() {
    sf::Vector2f distance(sf::Vector2f(this->mouseScreenPos) - sf::Vector2f(this->screenWidth / 2.f, this->screenHeight / 2.f + (float)this->pixelPerBlock));
    distance /= (float)this->pixelPerBlock;
    distance += playerPos;
    int chunkDistance = floor(distance.x / 16.f);
    distance.x = mod(distance.x, 16);
    this->mouseChunkID = this->playerChunkID + chunkDistance;
    this->mousePos.x = floor(distance.x);
    this->mousePos.y = floor(distance.y);
    this->updateHighlighterPosition();
}

void Chunks::updateHighlighterPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenWidth / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenHeight / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int i = this->mouseChunkID - this->chunksStartID;
    this->highlighter.setPosition(fChunkXPos + i * this->pixelPerBlock * 16.f + mousePos.x * this->pixelPerBlock, chunkYPos + (float)(mousePos.y * this->pixelPerBlock));
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
            if (std::filesystem::exists(std::format("saves/test/chunks/{}.dat", this->chunksEndID + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/test/chunks/{}.dat", this->chunksEndID + 1), this->pixelPerBlock, this->textureAtlas, this->atlasData));
            } else {
                this->chunks.push_back(Chunk(this->sampleChunk, this->pixelPerBlock, this->textureAtlas, this->atlasData));
            }
            this->chunks.front().saveToFile(std::format("saves/test/chunks/{}.dat", this->chunksStartID));
            this->chunks.pop_front();
            this->chunksStartID++;
            this->chunksEndID++;
        }
    } else {
        for (int i = 0; i < this->playerChunkID - chunkID; i++) {
            this->chunks.push_front(Chunk(this->sampleChunk, this->pixelPerBlock, this->textureAtlas, this->atlasData));
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

int Chunks::breakBlock() {
    int breakChunkIndex = this->mouseChunkID - this->chunksStartID;
    int xp;
    return this->chunks[breakChunkIndex].breakBlock(mousePos.x, mousePos.y, &xp);
}

}  // namespace mc