#ifndef MC_FURNACEINTERFACE_HPP
#define MC_FURNACEINTERFACE_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

#include "../include/json.hpp"
#include "mc_inventory.hpp"
#include "mc_progressBar.hpp"

using json = nlohmann::json;

namespace mc {

struct FurnaceData {
    ItemStack inputItemStack;
    ItemStack fuelItemStack;
    ItemStack outputItemStack;
    int progress;
    int fuelLeft;
    int fuelMax;
};

struct SmeltingRecipeData {
    int ingredientID;
    int resultID;
};

class FurnaceInterface : public sf::Drawable {
    Inventory inputInventory;
    Inventory fuelInventory;
    Inventory outputInventory;
    json& smeltingRecipesData;
    std::vector<SmeltingRecipeData> parsedSmeltingRecipesData;
    ProgressBar progressBar;
    ProgressBar fuelBar;

    FurnaceData furnaceData;

    void parseSmeltingRecipesData();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    FurnaceInterface(int scaling, sf::Font& font, sf::Texture& textureAtlas, json& atlasData, json& smeltingRecipesData, sf::Texture& progressBarTexture, sf::Texture& fuelBarTexture);
    void loadFromVector(std::vector<ItemStack> itemStacks);
    std::vector<ItemStack> exportToVector();
    void setInputPosition(sf::Vector2f position);
    void setFuelPosition(sf::Vector2f position);
    void setOutputPosition(sf::Vector2f position);
    void setProgressBarPosition(sf::Vector2f position);
    void setFuelBarPosition(sf::Vector2f position);
    void setScaling(int scaling);
    void setProgess(float progress);
    void setFuelVal(float fuelVal);
    sf::FloatRect getInputGlobalBounds();
    sf::FloatRect getInputLocalBounds();
    sf::FloatRect getInputSlotGlobalBounds(int slotID);
    sf::FloatRect getInputSlotLocalBounds(int slotID);
    sf::FloatRect getFuelGlobalBounds();
    sf::FloatRect getFuelLocalBounds();
    sf::FloatRect getFuelSlotGlobalBounds(int slotID);
    sf::FloatRect getFuelSlotLocalBounds(int slotID);
    sf::FloatRect getOutputGlobalBounds();
    sf::FloatRect getOutputLocalBounds();
    sf::FloatRect getOutputSlotGlobalBounds(int slotID);
    sf::FloatRect getOutputSlotLocalBounds(int slotID);
    ItemStack getInputItemStack(int slotID);
    ItemStack getFuelItemStack(int slotID);
    ItemStack getOutputItemStack(int slotID);
    int getInputEmptySpace(int slotID);
    int getFuelEmptySpace(int slotID);
    int subtractInputItem(int slotID, int amount);
    int subtractFuelItem(int slotID, int amount);
    int subtractOutputItem(int slotID, int amount);
    void setInputItemStack(int slotID, ItemStack itemStack);
    void setFuelItemStack(int slotID, ItemStack itemStack);
    ItemStack addInputItemStack(int slotID, ItemStack itemStack);
    ItemStack addFuelItemStack(int slotID, ItemStack itemStack);
    ItemStack takeOutputItem(int slotID);
    void setFurnaceData(FurnaceData furnaceData);
    FurnaceData getFurnaceData();
};

}  // namespace mc

#endif  // MC_FURNACEINTERFACE_HPP