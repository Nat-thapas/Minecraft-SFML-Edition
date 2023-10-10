#ifndef MC_CHUNKS_HPP
#define MC_CHUNKS_HPP

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

class Chunks : public sf::Drawable {
    int seed;
    std::string atlasFilesPath;
    std::string atlasDatasPath;
    sf::Texture textureAtlas;
    json atlasData;
    std::deque<mc::Chunk> chunks;
    Perlin noise;
    int pixelPerBlock;
    sf::Vector2i screenSize;
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
    Chunks(int playerChunkID, int seed, int pixelPerBlock, sf::Vector2i screenSize, std::string atlasFilesPath, std::string atlasDatasPath);
    void tick(int tickCount);
    void updateVertexArrays();
    void updateAnimatedVertexArrays();
    void updateLightLevels();
    void updateLightingVertexArray();
    void setPixelPerBlock(int pixelPerBlock);
    void setScreenSize(sf::Vector2i screenSize);
    int getPlayerChunkID();
    void setPlayerChunkID(int chunkID);
    sf::Vector2f getPlayerPos();
    void setPlayerPos(sf::Vector2f pos);
    void setMouseScreenPos(sf::Vector2i pos);
    int getMouseChunkID();
    sf::Vector2i getMousePos();
    sf::Vector2i getLoadedChunks();
    sf::Vector2i getPlayerLightLevel();
    sf::Vector2i getMouseLightLevel();
    int getBlock(int chunkID, int x, int y);
    int breakBlock(int& xp);
    bool placeBlock(int blockID);
    bool saveAll();
};

}  // namespace mc

#endif  // MC_CHUNKS_HPP