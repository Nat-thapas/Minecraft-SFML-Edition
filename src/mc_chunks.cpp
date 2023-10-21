#include <SFML/Graphics.hpp>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "idiv.hpp"
#include "mc_soundEffect.hpp"
#include "mod.hpp"

#include "mc_chunk.hpp"
#include "mc_chunks.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

void Chunks::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = &this->textureAtlas;
    states.shader = &this->chunkShader;
    for (const Chunk& chunk : this->chunks) {
        target.draw(chunk, states);
    }
    target.draw(this->highlighter, states);
    if (this->breakProgress > 0.f) {
        states.blendMode = sf::BlendMultiply;
        states.shader = &this->overlayShader;
        target.draw(this->breakProgressOverlay, states);
    }
}

void Chunks::updateTexture() {
    if (this->pixelPerBlock <= 16) {
        std::ifstream atlasDataFile(this->atlasDatasPath + std::format("blocksAtlasx{}.json", this->pixelPerBlock));
        this->atlasData = json::parse(atlasDataFile);
        this->textureAtlas.loadFromFile(this->atlasFilesPath + std::format("blocksAtlasx{}.png", this->pixelPerBlock));
        atlasDataFile.close();
    } else {
        std::ifstream atlasDataFile(this->atlasDatasPath + "blocksAtlasxF.json");
        this->atlasData = json::parse(atlasDataFile);
        this->textureAtlas.loadFromFile(this->atlasFilesPath + "blocksAtlasxF.png");
        atlasDataFile.close();
    }
}

void Chunks::parseAtlasData() {
    sf::IntRect texRect;
    for (int blockID = 0; blockID < 71; blockID++) {
        texRect.left = this->atlasData[std::format("{:03d}", blockID)]["x"];
        texRect.top = this->atlasData[std::format("{:03d}", blockID)]["y"];
        texRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
        texRect.height = this->atlasData[std::format("{:03d}", blockID)]["h"];
        this->parsedAtlasData[blockID] = texRect;
    }
}

void Chunks::parseSmeltingRecipesData() {
    size_t recipesCount = this->smeltingRecipesData.size();
    for (size_t i = 0; i < recipesCount; i++) {
        this->parsedSmeltingRecipesData[this->smeltingRecipesData[i]["ingredient"]] = this->smeltingRecipesData[i]["result_item"];
    }
}

void Chunks::initializeChunks() {
    for (int i = 0; i < this->chunkCountOnScreen; i++) {
        if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksStartID + i))) {
            this->chunks.push_back(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksStartID + i), this->chunksStartID + i, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
        } else {
            this->chunks.push_back(Chunk(this->noise, this->chunksStartID + i, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
        }
    }
}

Chunks::Chunks(int playerChunkID, int seed, int pixelPerBlock, std::string worldName, sf::Vector2i screenSize, std::string atlasFilesPath, std::string atlasDatasPath, std::string chunkShaderFilePath, std::string overlayShaderFilePath, json& smeltingRecipesData, std::string breakProgressOverlayTextureFilePath, SoundEffect& soundEffect) : smeltingRecipesData(smeltingRecipesData), soundEffect(soundEffect) {
    this->chunkShader.loadFromFile(chunkShaderFilePath, sf::Shader::Fragment);
    this->chunkShader.setUniform("time", 1.f);
    this->overlayShader.loadFromFile(overlayShaderFilePath, sf::Shader::Fragment);
    this->seed = seed;
    this->playerChunkID = playerChunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->worldName = worldName;
    this->screenSize = screenSize;
    this->atlasFilesPath = atlasFilesPath;
    this->atlasDatasPath = atlasDatasPath;
    this->chunksStartID = this->playerChunkID - static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunksEndID = this->playerChunkID + static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    Perlin noise(this->seed);
    this->noise = noise;
    this->breakProgressOverlayTexture.loadFromFile(breakProgressOverlayTextureFilePath);
    this->breakProgressOverlay.setTexture(this->breakProgressOverlayTexture);
    this->toolsBreakGroup[55] = 1;
    this->toolsBreakGroup[56] = 2;
    this->toolsBreakGroup[57] = 3;
    this->toolsBreakGroup[59] = 1;
    this->toolsBreakGroup[60] = 2;
    this->toolsBreakGroup[61] = 3;
    this->toolsBreakGroup[63] = 1;
    this->toolsBreakGroup[64] = 2;
    this->toolsBreakGroup[65] = 3;
    this->toolsBreakGroup[67] = 1;
    this->toolsBreakGroup[68] = 2;
    this->toolsBreakGroup[69] = 3;
    this->toolsBreakGroup[71] = 1;
    this->toolsBreakGroup[72] = 2;
    this->toolsBreakGroup[73] = 3;
    this->toolsMiningLevel[55] = 1;
    this->toolsMiningLevel[56] = 1;
    this->toolsMiningLevel[57] = 1;
    this->toolsMiningLevel[59] = 2;
    this->toolsMiningLevel[60] = 2;
    this->toolsMiningLevel[61] = 2;
    this->toolsMiningLevel[63] = 3;
    this->toolsMiningLevel[64] = 3;
    this->toolsMiningLevel[65] = 3;
    this->toolsMiningLevel[67] = 1;
    this->toolsMiningLevel[68] = 1;
    this->toolsMiningLevel[69] = 1;
    this->toolsMiningLevel[71] = 4;
    this->toolsMiningLevel[72] = 4;
    this->toolsMiningLevel[73] = 4;
    this->highlighter.setSize(sf::Vector2f(static_cast<float>(pixelPerBlock - 2), static_cast<float>(pixelPerBlock - 2)));
    this->highlighter.setOrigin(-1.f, -1.f);
    this->highlighter.setFillColor(sf::Color(0, 0, 0, 0));
    this->highlighter.setOutlineThickness(2.f);
    this->highlighter.setOutlineColor(sf::Color::Black);
    this->breakProgressOverlay.setScale(sf::Vector2f(static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->breakProgressOverlayTexture.getSize().y), static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->breakProgressOverlayTexture.getSize().y)));
    if (!std::filesystem::exists(std::format("saves/{}", this->worldName))) {
        std::filesystem::create_directory(std::format("saves/{}", this->worldName));
        std::filesystem::create_directory(std::format("saves/{}/chunks", this->worldName));
        std::filesystem::create_directory(std::format("saves/{}/inventories", this->worldName));
        std::filesystem::create_directory(std::format("saves/{}/inventories/chests", this->worldName));
        std::filesystem::create_directory(std::format("saves/{}/inventories/furnaces", this->worldName));
    }
    this->updateTexture();
    this->parseAtlasData();
    this->parseSmeltingRecipesData();
    this->initializeChunks();
}

bool Chunks::saveAll() {
    int i = chunksStartID;
    for (Chunk& chunk : this->chunks) {
        chunk.saveToFile();
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
    sf::Vector2f distance(sf::Vector2f(this->mouseScreenPos) - sf::Vector2f(this->screenSize.x / 2.f, this->screenSize.y / 2.f + static_cast<float>(this->pixelPerBlock)));
    distance /= static_cast<float>(this->pixelPerBlock);
    distance += playerPos;
    int chunkDistance = static_cast<int>(std::floor(distance.x / 16.f));
    distance.x = mod(distance.x, 16);
    this->mouseChunkID = this->playerChunkID + chunkDistance;
    this->mousePos.x = static_cast<int>(std::floor(distance.x));
    this->mousePos.y = static_cast<int>(std::floor(distance.y));
    this->updateHighlighterPosition();
}

void Chunks::updateHighlighterPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenSize.x / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenSize.y / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int chunkDist = this->mouseChunkID - this->chunksStartID;
    this->highlighter.setPosition(fChunkXPos + chunkDist * this->pixelPerBlock * 16.f + mousePos.x * this->pixelPerBlock, chunkYPos + static_cast<float>(mousePos.y * this->pixelPerBlock));
}

void Chunks::updateBreakOverlayPosition() {
    int lChunkDistance = this->playerChunkID - this->chunksStartID;
    float fChunkXPos = (this->screenSize.x / 2.f) - lChunkDistance * this->pixelPerBlock * 16.f - this->playerPos.x * this->pixelPerBlock;
    float chunkYPos = (this->screenSize.y / 2.f) - (this->playerPos.y * this->pixelPerBlock) + this->pixelPerBlock;
    int chunkDist = this->breakingChunkID - this->chunksStartID;
    this->breakProgressOverlay.setPosition(fChunkXPos + chunkDist * this->pixelPerBlock * 16.f + breakingPos.x * this->pixelPerBlock, chunkYPos + static_cast<float>(breakingPos.y * this->pixelPerBlock));
}

void Chunks::tick(int tickCount) {
    for (Chunk& chunk : this->chunks) {
        chunk.tick(tickCount);
    }
}

void Chunks::update() {
    for (Chunk& chunk : this->chunks) {
        chunk.update();
    }
}

void Chunks::updateVertexArrays() {
    for (Chunk& chunk : this->chunks) {
        chunk.updateVertexArray();
    }
}

void Chunks::updateAnimatedVertexArrays() {
    for (Chunk& chunk : this->chunks) {
        chunk.updateAnimatedVertexArray();
    }
}

void Chunks::updateLightLevels() {
    // for (Chunk& chunk : this->chunks) {
    //     chunk.updateLightLevels();
    // }
    int chunksCount = this->chunks.size();
    for (int i = 0; i < chunksCount; i++) {
        this->chunks[i].updateLightLevels();
        if (chunksCount - i > 1) {
            this->chunks[i+1].setLeftChunkSkyLightLevels(this->chunks[i].getRightSkyLightLevels());
            this->chunks[i+1].setLeftChunkBlockLightLevels(this->chunks[i].getRightBlockLightLevels());
            this->chunks[i+1].updateLightLevels();
            this->chunks[i].setRightChunkSkyLightLevels(this->chunks[i+1].getLeftSkyLightLevels());
            this->chunks[i].setRightChunkBlockLightLevels(this->chunks[i+1].getLeftBlockLightLevels());
            this->chunks[i].updateLightLevels();
        }
    }
}

void Chunks::updateLightingVertexArray() {
    for (Chunk& chunk : this->chunks) {
        chunk.updateLightingVertexArray();
    }
}

void Chunks::setPixelPerBlock(int pixelPerBlock) {
    if (this->pixelPerBlock == pixelPerBlock) {
        return;
    }
    this->pixelPerBlock = pixelPerBlock;
    this->updateTexture();
    this->parseAtlasData();
    if (this->pixelPerBlock > 2) {
        this->highlighter.setSize(sf::Vector2f(static_cast<float>(this->pixelPerBlock - 2), static_cast<float>(this->pixelPerBlock - 2)));
    } else {
        this->highlighter.setSize(sf::Vector2f(static_cast<float>(this->pixelPerBlock), static_cast<float>(this->pixelPerBlock)));
    }
    this->breakProgressOverlay.setScale(sf::Vector2f(static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->breakProgressOverlayTexture.getSize().y), static_cast<float>(this->pixelPerBlock) / static_cast<float>(this->breakProgressOverlayTexture.getSize().y)));
    for (Chunk& chunk : this->chunks) {
        chunk.setPixelPerBlock(this->pixelPerBlock);
    }
    int oldChunksStartID = this->chunksStartID;
    int oldChunksEndID = this->chunksEndID;
    this->chunksStartID = this->playerChunkID - static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunksEndID = this->playerChunkID + static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    if (this->chunksEndID > oldChunksEndID) {
        for (int i = 0; i < this->chunksEndID - oldChunksEndID; i++) {
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksEndID + i + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksEndID + i + 1), oldChunksEndID + i + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_back(Chunk(this->noise, oldChunksEndID + i + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
        }
    } else if (this->chunksEndID < oldChunksEndID) {
        for (int i = 0; i < oldChunksEndID - this->chunksEndID; i++) {
            this->chunks.back().saveToFile();
            this->chunks.pop_back();
        }
    }
    if (this->chunksStartID < oldChunksStartID) {
        for (int i = 0; i < oldChunksStartID - this->chunksStartID; i++) {
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksStartID - i - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksStartID - i - 1), oldChunksStartID - i - 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_front(Chunk(this->noise, oldChunksStartID - i - 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
        }
    } else if (this->chunksStartID > oldChunksStartID) {
        for (int i = 0; i < this->chunksStartID - oldChunksStartID; i++) {
            this->chunks.front().saveToFile();
            this->chunks.pop_front();
        }
    }
    this->updateChunksPosition();
    this->updateMousePosition();
    this->updateBreakOverlayPosition();
}

void Chunks::setScreenSize(sf::Vector2i screenSize) {
    if (this->screenSize == screenSize) {
        return;
    }
    this->screenSize = screenSize;
    int oldChunksStartID = this->chunksStartID;
    int oldChunksEndID = this->chunksEndID;
    this->chunksStartID = this->playerChunkID - static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunksEndID = this->playerChunkID + static_cast<int>(std::ceil((this->screenSize.x / 2.f) / (16.f * this->pixelPerBlock)));
    this->chunkCountOnScreen = this->chunksEndID - this->chunksStartID + 1;
    if (this->chunksEndID > oldChunksEndID) {
        for (int i = 0; i < this->chunksEndID - oldChunksEndID; i++) {
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksEndID + i + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksEndID + i + 1), oldChunksEndID + i + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_back(Chunk(this->noise, oldChunksEndID + i + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
        }
    } else if (this->chunksEndID < oldChunksEndID) {
        for (int i = 0; i < oldChunksEndID - this->chunksEndID; i++) {
            this->chunks.back().saveToFile();
            this->chunks.pop_back();
        }
    }
    if (this->chunksStartID < oldChunksStartID) {
        for (int i = 0; i < oldChunksStartID - this->chunksStartID; i++) {
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksStartID - i - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, oldChunksStartID - i - 1), oldChunksStartID - i - 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_front(Chunk(this->noise, oldChunksStartID - i - 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
        }
    } else if (this->chunksStartID > oldChunksStartID) {
        for (int i = 0; i < this->chunksStartID - oldChunksStartID; i++) {
            this->chunks.front().saveToFile();
            this->chunks.pop_front();
        }
    }
    this->updateChunksPosition();
    this->updateMousePosition();
    this->updateBreakOverlayPosition();
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
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksEndID + 1))) {
                this->chunks.push_back(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksEndID + 1), this->chunksEndID + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_back(Chunk(this->noise, this->chunksEndID + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
            this->chunks.front().saveToFile();
            this->chunks.pop_front();
            this->chunksStartID++;
            this->chunksEndID++;
        }
    } else {
        for (int i = 0; i < this->playerChunkID - chunkID; i++) {
            if (std::filesystem::exists(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksStartID - 1))) {
                this->chunks.push_front(Chunk(std::format("saves/{}/chunks/{}.dat.gz", this->worldName, this->chunksStartID - 1), this->chunksStartID + 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            } else {
                this->chunks.push_front(Chunk(this->noise, this->chunksStartID - 1, this->pixelPerBlock, this->worldName, this->parsedAtlasData, this->parsedSmeltingRecipesData, this->soundEffect));
            }
            this->chunks.back().saveToFile();
            this->chunks.pop_back();
            this->chunksStartID--;
            this->chunksEndID--;
        }
    }
    this->playerChunkID = chunkID;
    this->updateChunksPosition();
    this->updateMousePosition();
    this->updateBreakOverlayPosition();
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
    this->updateBreakOverlayPosition();
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

sf::Vector2i Chunks::getPlayerLightLevel() {
    if (this->playerChunkID < this->chunksStartID || this->playerChunkID > this->chunksEndID) {
        return sf::Vector2i(-1, -1);
    }
    sf::Vector2i playerIntPos;
    playerIntPos.x = static_cast<int>(this->playerPos.x);
    playerIntPos.y = static_cast<int>(std::ceil(this->playerPos.y)) - 1;
    if (playerIntPos.x < 0 || playerIntPos.x > 15 || playerIntPos.y < 0 || playerIntPos.y > 255) {
        return sf::Vector2i(-1, -1);
    }
    return sf::Vector2i(this->chunks[this->playerChunkID - this->chunksStartID].getSkyLightLevel(playerIntPos.x, playerIntPos.y), this->chunks[this->playerChunkID - this->chunksStartID].getBlockLightLevel(playerIntPos.x, playerIntPos.y));
}

sf::Vector2i Chunks::getMouseLightLevel() {
    if (this->mouseChunkID < this->chunksStartID || this->mouseChunkID > this->chunksEndID) {
        return sf::Vector2i(-1, -1);
    }
    if (mousePos.x < 0 || mousePos.x > 15 || mousePos.y < 0 || mousePos.y > 255) {
        return sf::Vector2i(-1, -1);
    }
    return sf::Vector2i(this->chunks[this->mouseChunkID - this->chunksStartID].getSkyLightLevel(mousePos.x, mousePos.y), this->chunks[this->mouseChunkID - this->chunksStartID].getBlockLightLevel(mousePos.x, mousePos.y));
}

int Chunks::getBlock(int chunkID, int x, int y) {
    if (chunkID < this->chunksStartID || chunkID > this->chunksEndID) {
        return 0;
    }
    if (x < 0 || x > 15 || y < 0 || y > 255) {
        return 0;
    }
    return this->chunks[chunkID - this->chunksStartID].getBlock(x, y);
}

void Chunks::setBlock(int chunkID, int x, int y, int blockID) {
    if (chunkID < this->chunksStartID || chunkID > this->chunksEndID) {
        return;
    }
    if (x < 0 || x > 15 || y < 0 || y > 255) {
        return;
    }
    this->chunks[chunkID - this->chunksStartID].setBlock(x, y, blockID);
}

std::vector<ItemStack> Chunks::breakBlock(int itemID) {
    if (this->mouseChunkID < this->chunksStartID || this->mouseChunkID > this->chunksEndID) {
        return std::vector<ItemStack>({ItemStack(0, 0)});
    }
    if (this->mousePos.x < 0 || this->mousePos.x > 15 || this->mousePos.y < 0 || this->mousePos.y > 255) {
        return std::vector<ItemStack>({ItemStack(0, 0)});
    }
    sf::Vector2f playerCenter(std::floor(this->playerPos.x), std::ceil(this->playerPos.y) - 1.5f);
    sf::Vector2f mousePosF(this->mousePos);
    sf::Vector2f dist = mousePosF - playerCenter;
    dist.x += static_cast<float>(this->mouseChunkID - this->playerChunkID) * 16.f;
    if (dist.y >= 0) {
        dist.y -= 0.5f;
        std::max(dist.y, 0.f);
    } else {
        dist.y += 0.5f;
        std::min(dist.y, 0.f);
    }
    if (sqrtf(dist.x * dist.x + dist.y * dist.y) > 3.f) {
        return std::vector<ItemStack>({ItemStack(0, 0)});
    }
    int breakChunkIndex = this->mouseChunkID - this->chunksStartID;
    int breakBlockID = this->chunks[breakChunkIndex].getBlock(this->mousePos.x, this->mousePos.y);
    if (!this->isBlockBreakable[breakBlockID]) {
        return std::vector<ItemStack>({ItemStack(0, 0)});
    }
    if (this->breakingChunkID != this->mouseChunkID || this->breakingPos != this->mousePos) {
        this->breakProgress = 0.f;
        this->breakingChunkID = this->mouseChunkID;
        this->breakingPos = this->mousePos;
    }
    bool harvestable = false;
    float speedMultiplier = 1.f;
    int blockBreakGroup = this->blocksBreakGroup[breakBlockID];
    int toolBreakGroup = this->toolsBreakGroup.contains(itemID) ? this->toolsBreakGroup[itemID] : 0;
    int toolMingingLevel = this->toolsMiningLevel.contains(itemID) ? this->toolsMiningLevel[itemID] : 0;
    int toolSpeedMultiplier = this->breakSpeedMultipliers[itemID];
    float blockHardness = this->blocksHardness[breakBlockID];
    int requiredMiningLevel = this->blocksMiningLevel[breakBlockID];
    if (requiredMiningLevel) {
        harvestable = blockBreakGroup == toolBreakGroup && toolMingingLevel >= requiredMiningLevel;
    } else {
        harvestable = true;
    }
    if (toolBreakGroup && blockBreakGroup && blockBreakGroup == toolBreakGroup) {
        speedMultiplier *= static_cast<float>(toolSpeedMultiplier);
    }
    if (blockHardness > 0.f) {
        this->breakProgress += speedMultiplier / blockHardness / (harvestable ? 30.f : 100.f);
    } else {
        this->breakProgress = 1.f;
    }
    int textureSize = this->breakProgressOverlayTexture.getSize().y;
    int stagesCount = this->breakProgressOverlayTexture.getSize().x / textureSize;
    int stage = static_cast<int>(std::floor(this->breakProgress * static_cast<float>(stagesCount)));
    stage = std::min(stage, stagesCount - 1);
    sf::IntRect textureRect;
    textureRect.left = textureSize * stage;
    textureRect.top = 0;
    textureRect.width = textureSize;
    textureRect.height = textureSize;
    this->breakProgressOverlay.setTextureRect(textureRect);
    this->updateBreakOverlayPosition();
    if (this->breakProgress >= 1.f) {
        this->breakProgress = 0.f;
        std::vector<ItemStack> droppedItemStacks;
        if (breakBlockID == 40) {
            std::string filePath = std::format("saves/{}/inventories/chests/{}.{}.{}.dat.gz", this->worldName, this->breakingChunkID, this->breakingPos.x, this->breakingPos.y);
            if (std::filesystem::exists(filePath)) {
                std::vector<ItemStack> tempDroppedItemStacks = Inventory::getDataFromFile(filePath, 27);
                droppedItemStacks.insert(droppedItemStacks.end(), tempDroppedItemStacks.begin(), tempDroppedItemStacks.end());
                std::filesystem::remove(filePath);
            }
        }
        if (breakBlockID == 41 || breakBlockID == 42) {
            std::string filePath = std::format("saves/{}/inventories/furnaces/{}.{}.{}.dat.gz", this->worldName, this->breakingChunkID, this->breakingPos.x, this->breakingPos.y);
            if (std::filesystem::exists(filePath)) {
                std::filesystem::remove(filePath);
            }
            FurnaceData furnaceData = this->chunks[breakingChunkID - this->chunksStartID].getFurnaceData(this->breakingPos.x, this->breakingPos.y);
            droppedItemStacks.push_back(furnaceData.inputItemStack);
            droppedItemStacks.push_back(furnaceData.fuelItemStack);
            droppedItemStacks.push_back(furnaceData.outputItemStack);
        }
        int dropData = this->chunks[breakChunkIndex].breakBlock(mousePos.x, mousePos.y);
        if (!harvestable) {
            return droppedItemStacks;
        }
        if (dropData == 1136) {
            droppedItemStacks.push_back(ItemStack(112, 4));
            return droppedItemStacks;
        }
        if (dropData == 5701668) {
            droppedItemStacks.push_back(ItemStack(36, 1));
            droppedItemStacks.push_back(ItemStack(87, 1));
            return droppedItemStacks;
        }
        droppedItemStacks.push_back(ItemStack(dropData, 1));
        return droppedItemStacks;
    } else {
        return std::vector<ItemStack>({ItemStack(0, 0)});
    }
}

Chunk& Chunks::getChunk(int chunkID) {
    assert(chunkID >= this->chunksStartID && chunkID <= this->chunksEndID);
    return this->chunks[chunkID - this->chunksStartID];
}

bool Chunks::placeBlock(int itemID) {
    if (this->mouseChunkID < this->chunksStartID || this->mouseChunkID > this->chunksEndID) {
        return false;
    }
    if (this->mousePos.x < 0 || this->mousePos.x > 15 || this->mousePos.y < 0 || this->mousePos.y > 255) {
        return false;
    }
    if (this->mousePos.y == static_cast<int>(std::ceil(this->playerPos.y)) - 1 || this->mousePos.y == static_cast<int>(std::ceil(this->playerPos.y - 1.f)) - 1 || this->mousePos.y == static_cast<int>(std::ceil(this->playerPos.y - 1.8f)) - 1) {
        if (this->mousePos.x == static_cast<int>(std::floor(this->playerPos.x - 0.3f)) || this->mousePos.x == static_cast<int>(std::floor(this->playerPos.x)) || this->mousePos.x == static_cast<int>(std::floor(this->playerPos.x - 0.3f))) {
            return false;
        }
    }
    sf::Vector2f playerCenter(std::floor(this->playerPos.x), std::ceil(this->playerPos.y) - 1.5f);
    sf::Vector2f mousePosF(this->mousePos);
    sf::Vector2f dist = mousePosF - playerCenter;
    dist.x += static_cast<float>(this->mouseChunkID - this->playerChunkID) * 16.f;
    if (dist.y >= 0) {
        dist.y -= 0.5f;
        std::max(dist.y, 0.f);
    } else {
        dist.y += 0.5f;
        std::min(dist.y, 0.f);
    }
    if (sqrtf(dist.x * dist.x + dist.y * dist.y) > 3.f) {
        return false;
    }
    int placeChunkIndex = this->mouseChunkID - this->chunksStartID;
    return this->chunks[placeChunkIndex].placeBlock(mousePos.x, mousePos.y, itemID);
}

}  // namespace mc