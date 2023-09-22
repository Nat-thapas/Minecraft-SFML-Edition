#ifndef MC_CHUNKS_HPP
#define MC_CHUNKS_HPP

#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "mc_chunk.hpp"
#include "mod.hpp"
#include "idiv.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

class Chunks : public sf::Drawable {
    int seed;
    std::string atlasFilePath;
    std::string atlasDataPath;
    sf::Texture textureAtlas;
    json atlasData;
    std::deque<mc::Chunk> chunks;
    Perlin noise;
    int pixelPerBlock;
    int screenWidth;
    int screenHeight;
    int chunkCountOnScreen;
    int chunksStartID;
    int chunksEndID;
    int playerChunkID;
    sf::Vector2f playerPos;
    sf::Vector2i mouseScreenPos;
    int mouseChunkID;
    sf::Vector2i mousePos;
    sf::RectangleShape highlighter;

    void updateTexture();
    void updateChunksPosition();
    void initializeChunks();
    void updateMousePosition();
    void updateHighlighterPosition();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunks(int playerChunkID, int seed, int pixelPerBlock, int screenWidth, int screenHeight, std::string atlasFilePath, std::string atlasDataPath);
    void tick(int tickCount);
    void updateVertexArrays();
    void setPixelPerBlock(int pixelPerBlock);
    int getPlayerChunkID();
    void setPlayerChunkID(int chunkID);
    sf::Vector2f getPlayerPos();
    void setPlayerPos(sf::Vector2f pos);
    void setMouseScreenPos(sf::Vector2i pos);
    int getMouseChunkID();
    sf::Vector2i getMousePos();
    sf::Vector2i getLoadedChunks();
    int breakBlock(int& xp);
    bool saveAll();
};

}  // namespace mc

#endif  // MC_CHUNKS_HPP