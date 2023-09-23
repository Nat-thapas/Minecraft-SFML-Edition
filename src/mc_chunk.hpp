#ifndef MC_CHUNK_HPP
#define MC_CHUNK_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstdlib>
#include <format>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

class Chunk : public sf::Drawable, public sf::Transformable {
    int chunkID;
    sf::Texture& textureAtlas;
    json& atlasData;
    sf::VertexArray vertexArray;
    int pixelPerBlock;
    std::queue<int> vertexUpdateQueue;
    std::queue<int> blockUpdateQueue;
    int blocks[4096];
    int itemDropIDs[71] = {0, 4, 3, 3, 4, 5, 6, 5, 8, 87, 0, 0, 0, 0, 0, 11, 12, 13, 14, 15, 49, 17, 18, 53, 0, 21, 0, 22, 23, 24, 25, 26, 27, 28, 29, 112, 31, 32, 0, 33, 34, 35, 35, 87, 87, 87, 87, 87, 87, 87, (87 << 8) + 36, 3, 3, 37, 0, 38, 0, 38, 39, 40, 41, 42, 43, 0, 44, 45, 0, 0, 0, 47, 48};
    int itemDropChances[71] = {0, 100, 100, 100, 100, 100, 100, 15, 100, 25, 0, 0, 0, 0, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 0, 100, 100, 0, 0, 0, 100, 100};
    int experienceDropAmount[71] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int blockPlaceIDs[123] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 40, 41, 0, 53, 55, 58, 59, 60, 61, 62, 64, 65, 67, 69, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int animationIndex;

    void initializeVertexArray();
    void updateAnimatedVertexArray();
    void updateAllVertexArray();
    void update();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunk(int blocks[4096], int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData);
    Chunk(std::string, int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData);
    Chunk(Perlin& noise, int chunkID, int pixelPerBlock, sf::Texture& textureAtlas, json& atlasData);
    void setPixelPerBlock(int pixelPerBlock);
    int getBlock(int x, int y);
    void setBlock(int x, int y, int blockID);
    bool placeBlock(int x, int y, int itemID);
    int breakBlock(int x, int y, int& xp);
    void tick(int tickCount);
    void updateVertexArray();
    bool saveToFile(std::string filePath);
};

}  // namespace mc

#endif  // MC_CHUNK_HPP