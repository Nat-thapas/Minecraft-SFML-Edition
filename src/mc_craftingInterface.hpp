#ifndef MC_CRAFTINGINTERFACE_HPP
#define MC_CRAFTINGINTERFACE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "../include/json.hpp"
#include "mc_inventory.hpp"

using json = nlohmann::json;

namespace mc {

struct RecipeData {
    int width;
    int height;
    bool shaped;
    std::vector<int> ingredients;
    ItemStack resultItemStack;
};

class CraftingInterface : public sf::Drawable {
    int size;
    Inventory inputInventory;
    Inventory outputInventory;
    json& recipesData;
    std::vector<RecipeData> parsedRecipesData;

    void parseRecipesData();
    sf::IntRect getRecipeRect();
    std::vector<int> getMatchVector();
    std::vector<int> getShapelessMatchVector(std::vector<int> matchVector);
    void updateOutput();
    void setOutputItemStack(int slotID, ItemStack itemStack);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    CraftingInterface(int size, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData, json& recipesData);
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
    int getInputEmptySpace(int slotID);
    int subtractInputItem(int slotID, int amount);
    void setInputItemStack(int slotID, ItemStack itemStack);
    ItemStack addInputItemStack(int slotID, ItemStack itemStack);
    ItemStack takeOutputItem(int slotID);
};

}  // namespace mc

#endif  // MC_CRAFTINGINTERFACE_HPP