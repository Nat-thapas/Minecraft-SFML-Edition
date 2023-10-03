#ifndef MC_INVENTORY_HPP
#define MC_INVENTORY_HPP

#include <SFML/Graphics.hpp>
#include <format>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;

namespace mc {

struct ItemStack {
    int amount;
    int id;
};
    
class Inventory : public sf::Drawable, sf::Transformable {
    int size;
    int width;
    std::vector<ItemStack> itemStacks;
    sf::Texture& textureAtlas;
    json& atlasData;
    std::array<sf::IntRect, 123> parsedAtlasData;
    sf::VertexArray vertexArray;
    int scaling;
    std::array<int, 123> stackSizes = {0, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 1, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 16, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 64, 1, 64, 64, 64, 1, 1, 1, 1, 1, 1, 64, 64, 64, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 16, 1, 1, 64, 64, 64, 64, 16, 64, 64, 64, 64, 16, 64, 64};

    void parseAtlasData();
    void initializeVertexArray();
    void updateAllVertexArray();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Inventory(int size, int width, int scaling, sf::Texture& textureAtlas, json& atlasData);
    ItemStack getItemStack(int slotID);
    void setItemStack(int slotID, ItemStack itemStack);
    ItemStack addItemStack(ItemStack itemStack);
};

}  // namespace mc

#endif  // MC_INVENTORY_HPP