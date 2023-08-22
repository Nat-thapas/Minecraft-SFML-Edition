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
    int blocks[4096];
    int animationIndex;
    json atlasData;

    void initializeVertexArray();
    void updateAnimatedVertexArray();
    void updateAllVertexArray();
    void updateVertexArray();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunk(int blocks[4096], std::string textureAtlasFileName, std::string atlasDataFileName);
    int getBlock(int x, int y);
    void setBlock(int x, int y, int blockID);
    void tickAnimation();
};

}  // namespace mc

#endif  // MC_CHUNK_HPP