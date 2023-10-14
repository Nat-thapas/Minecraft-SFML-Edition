#ifndef MC_CRAFTINGINTERFACE_HPP
#define MC_CRAFTINGINTERFACE_HPP

#include <SFML/Graphics.hpp>

#include "../include/json_fwd.hpp"
#include "mc_inventory.hpp"

using json = nlohmann::json;

namespace mc {

class CraftingInterface : public sf::Drawable {
    Inventory inputInventory;
    Inventory outputInventory;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    CraftingInterface(int size, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData);
    void setInputPosition(sf::Vector2f position);
    void setOutputPosition(sf::Vector2f position);
    void setScaling(int scaling);
    void setMargin(int margin);
    sf::FloatRect getInputGlobalBounds();
    sf::FloatRect getInputLocalBounds();
    sf::FloatRect getInputSlotGlobalBounds(int slotID);
    sf::FloatRect getInputSlotLocalBounds(int slotID);
    sf::FloatRect getOutputGlobalBounds();
    sf::FloatRect getOutputLocalBounds();
    sf::FloatRect getOutputSlotGlobalBounds(int slotID);
    sf::FloatRect getOutputSlotLocalBounds(int slotID);
    ItemStack getInputItemStack(int slotID);
    ItemStack getOutputItemStack(int slotID);
    void setInputItemStack(int slotID, ItemStack itemStack);
    ItemStack addInputItemStack(int slotID, ItemStack itemStack);
    int subtractOutputItem(int slotID, int amount);
};

}  // namespace mc

#endif  // MC_CRAFTINGINTERFACE_HPP