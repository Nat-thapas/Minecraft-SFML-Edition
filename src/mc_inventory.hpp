#ifndef MC_INVENTORY_HPP
#define MC_INVENTORY_HPP

#include <SFML/Graphics.hpp>

#include "../include/json_fwd.hpp"

using json = nlohmann::json;

namespace mc {

struct ItemStack {
    int amount;
    int id;
};
    
class Inventory : public sf::Drawable, public sf::Transformable {
    int size;
    int width;
    int scaling;
    int margin;
    sf::Font& font;
    std::vector<ItemStack> itemStacks;
    sf::Texture& textureAtlas;
    json& atlasData;
    std::vector<sf::Text> amountLabels;
    std::array<sf::IntRect, 123> parsedAtlasData;
    sf::VertexArray vertexArray;
    std::array<int, 123> stackSizes = {0, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 1, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 16, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 64, 1, 64, 64, 64, 1, 1, 1, 1, 1, 1, 64, 64, 64, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 64, 64, 16, 1, 1, 64, 64, 64, 64, 16, 64, 64, 64, 64, 16, 64, 64};

    void parseAtlasData();
    void initializeVertexArray();
    void updateAllVertexArray();
    void initializeAmountLabels();
    void updateAllAmountLabels();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Inventory(int size, int width, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData);
    void setScaling(int scaling);
    void setMargin(int margin);
    sf::FloatRect getGlobalBounds();
    sf::FloatRect getLocalBounds();
    sf::FloatRect getSlotGlobalBounds(int slotID);
    sf::FloatRect getSlotLocalBounds(int slotID);
    ItemStack getItemStack(int slotID);
    void setItemStack(int slotID, ItemStack itemStack);
    ItemStack addItemStack(ItemStack itemStack);
    int subtractItem(int slotID, int amount);
};

}  // namespace mc

#endif  // MC_INVENTORY_HPP