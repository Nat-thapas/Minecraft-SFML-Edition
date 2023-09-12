#include "mc_chunks.hpp"

#include <cstdlib>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>

#include "mod.hpp"
#include "idiv.hpp"

namespace mc {

void Chunks::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (mc::Chunk chunk : this->chunks) {
        target.draw(chunk, states);
    }
}

Chunks::Chunks(int playerChunkID, int pixelPerBlock, int screenWidth, std::string atlasFilePath, std::string atlasDataPath) {
    this->playerChunkID = playerChunkID;
    this->pixelPerBlock = pixelPerBlock;
    this->screenWidth = screenWidth;
    this->atlasFilePath = atlasFilePath;
    this->atlasDataPath = atlasDataPath;
    this->chunkCountOnScreen = idiv(screenWidth, pixelPerBlock*16) + 1;
}

int Chunks::getPlayerChunkID() {
    return this->playerChunkID;
}

void Chunks::setPlayerChunkID(int chunkID) {
    this->playerChunkID = chunkID;
}

sf::Vector2f Chunks::getPlayerPos() {
    return this->playerPos;
}

void Chunks::setPlayerPos(sf::Vector2f pos) {
    this->playerPos = pos;
}

}  // namespace mc