#ifndef MC_CHUNK_HPP
#define MC_CHUNK_HPP

#include <format>
#include <fstream>
#include <string>
#include <vector>

#include <include/json.hpp>
#include <SFML/Graphics.hpp>

#include "mod.hpp"
#include "idiv.hpp"

using json = nlohmann::json;

namespace mc {

class Chunk : public sf::Drawable, public sf::Transformable {
    sf::Texture textureAtlas;
    sf::VertexArray vertexArray;
    std::vector<int> blocks;
    json atlasData;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunk(std::vector<int> blocks, std::string textureAtlasFileName, std::string atlasDataFileName);
    void updateVertexArray();
    int getBlock(int x, int y);
    void setBlock(int x, int y, int blockID);
};

}  // namespace mc

#endif  // MC_CHUNK_HPP