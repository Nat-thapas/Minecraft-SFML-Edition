#ifndef MC_CHUNK_HPP
#define MC_CHUNK_HPP

#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

#include <SFML/Graphics.hpp>

#include "../include/json.hpp"
#include "mod.hpp"
#include "idiv.hpp"

using json = nlohmann::json;

namespace mc {

class Chunk : public sf::Drawable, public sf::Transformable {
    sf::Texture textureAtlas;
    sf::VertexArray vertexArray;
    std::queue<int> vertexUpdateQueue;
    std::queue<int> blockUpdateQueue;
    int blocks[4096];
    int itemDropIDs[71] = {0, 4, 3, 3, 4, 5, 6, 5, 8, 87, 0, 0, 0, 0, 0, 11, 12, 13, 14, 15, 49, 17, 18, 53, 0, 21, 0, 22, 23, 24, 25, 26, 27, 28, 29, 112, 31, 32, 0, 33, 34, 35, 35, 87, 87, 87, 87, 87, 87, 87, (87 << 8) + 36, 3, 3, 37, 0, 38, 0, 38, 39, 40, 41, 42, 43, 0, 44, 45, 0, 0, 0, 47, 48};
    int itemDropChances[71] = {0, 100, 100, 100, 100, 100, 100, 15, 100, 25, 0, 0, 0, 0, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0, 100, 0, 100, 100, 100, 100, 100, 100, 0, 100, 100, 0, 0, 0, 100, 100};
    int experienceDropAmount[71] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int blockPlaceIDs[123] = {}; // TODO
    int animationIndex;
    json atlasData;

    void initializeVertexArray();
    void updateAnimatedVertexArray();
    void updateAllVertexArray();
    void updateVertexArray();
    void update();
    void setBlock(int x, int y, int blockID);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunk(int blocks[4096], std::string textureAtlasFileName, std::string atlasDataFileName);
    int getBlock(int x, int y);
    bool placeBlock(int x, int y, int itemID);
    int breakBlock(int x, int y, int *xp);
    void tickAnimation();
};

}  // namespace mc

#endif  // MC_CHUNK_HPP