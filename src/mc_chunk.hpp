#ifndef MC_CHUNK_HPP
#define MC_CHUNK_HPP

#include <SFML/Graphics.hpp>
#include <array>
#include <queue>
#include <string>

#include "../include/perlin.hpp"

using Perlin = siv::PerlinNoise;

namespace mc {

class Chunk : public sf::Drawable, public sf::Transformable {
    int chunkID;
    std::array<sf::IntRect, 71>& parsedAtlasData;
    sf::VertexArray vertexArray;
    int pixelPerBlock;
    std::queue<int> vertexUpdateQueue;
    std::queue<int> blockUpdateQueue;
    std::queue<int> skyLightUpdateQueue;
    std::queue<int> blockLightUpdateQueue;
    std::queue<int> lightingVertexUpdateQueue;
    std::array<int, 4096> blocks;
    std::array<int, 4096> skyLightLevels;
    std::array<int, 4096> blockLightLevels;
    std::array<int, 256> leftChunkSkyLightLevels = {};
    std::array<int, 256> rightChunkSkyLightLevels = {};
    std::array<int, 256> leftChunkBlockLightLevels = {};
    std::array<int, 256> rightChunkBlockLightLevels = {};
    std::array<int, 71> itemDropIDs = {0, 4, 3, 3, 4, 5, 6, 5, 8, 87, 0, 0, 0, 0, 0, 11, 12, 13, 14, 15, 49, 17, 18, 53, 0, 21, 0, 22, 23, 24, 25, 26, 27, 28, 29, 112, 31, 32, 0, 33, 34, 35, 35, 87, 87, 87, 87, 87, 87, 87, (87 << 8) + 36, 3, 3, 37, 0, 38, 0, 38, 39, 40, 41, 42, 43, 0, 44, 45, 0, 0, 0, 47, 48};
    std::array<int, 71> itemDropChances = {0, 100, 100, 100, 100, 100, 100, 15, 100, 25, 0, 0, 0, 0, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 0, 100, 100, 0, 0, 0, 100, 100};
    std::array<int, 71> experienceDropAmount = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<int, 123> blockPlaceIDs = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 40, 41, 0, 53, 55, 58, 59, 60, 61, 62, 64, 65, 67, 69, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::array<bool, 71> blockOpaqueness = {0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0};
    std::array<int, 71> blockEmissionLevels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 15, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 15, 0, 0, 0, 0};
    std::array<std::array<int, 35>, 3> oakTrees = {{
        {0, 7, 7, 7, 0,
         7, 7, 7, 7, 7,
         7, 7, 6, 7, 7,
         7, 7, 6, 7, 7,
         0, 0, 6, 0, 0,
         0, 0, 6, 0, 0,
         0, 0, 6, 0, 0},
        {0, 0, 0, 0, 0,
         0, 0, 7, 0, 0,
         0, 7, 7, 7, 0,
         7, 7, 6, 7, 7,
         7, 7, 6, 7, 7,
         0, 0, 6, 0, 0,
         0, 0, 6, 0, 0},
        {0, 0, 0, 0, 0,
         0, 0, 0, 0, 0,
         0, 7, 7, 7, 0,
         7, 7, 7, 7, 7,
         7, 7, 6, 7, 7,
         0, 7, 6, 7, 0,
         0, 0, 6, 0, 0},
    }};
    int animationIndex;

    void initializeVertexArrays();
    void updateAllVertexArray();
    sf::Color getColorFromLightLevel(int lightLevel);
    bool isBlockOpaque(int x, int y);
    int getBlockEmissionLevel(int x, int y);
    void initializeLightEngine();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunk(int blocks[4096], int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData);
    Chunk(std::string, int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData);
    Chunk(Perlin& noise, int chunkID, int pixelPerBlock, std::array<sf::IntRect, 71>& parsedAtlasData);
    void update();
    void setPixelPerBlock(int pixelPerBlock);
    int getBlock(int x, int y);
    void setBlock(int x, int y, int blockID);
    bool placeBlock(int x, int y, int itemID);
    int breakBlock(int x, int y, int& xp);
    void tick(int tickCount);
    int getSkyLightLevel(int x, int y);
    int getBlockLightLevel(int x, int y);
    int getLightLevel(int x, int y);
    void setLeftChunkSkyLightLevels(std::array<int, 256> lightLevels);
    void setRightChunkSkyLightLevels(std::array<int, 256> lightLevels);
    void setLeftChunkBlockLightLevels(std::array<int, 256> lightLevels);
    void setRightChunkBlockLightLevels(std::array<int, 256> lightLevels);
    std::array<int, 256> getLeftSkyLightLevels();
    std::array<int, 256> getRightSkyLightLevels();
    std::array<int, 256> getLeftBlockLightLevels();
    std::array<int, 256> getRightBlockLightLevels();
    void updateVertexArray();
    void updateAnimatedVertexArray();
    void updateLightLevels();
    void updateAllLightingVertexArray();
    void updateLightingVertexArray();
    bool saveToFile(std::string filePath);
};

}  // namespace mc

#endif  // MC_CHUNK_HPP