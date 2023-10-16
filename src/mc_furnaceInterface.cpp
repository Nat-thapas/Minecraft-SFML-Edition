#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

#include "../include/json.hpp"
#include "mc_inventory.hpp"

#include "mc_furnaceInterface.hpp"

using json = nlohmann::json;

namespace mc {

void FurnaceInterface::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->inputInventory, states);
    target.draw(this->fuelInventory, states);
    target.draw(this->outputInventory, states);
}

FurnaceInterface::FurnaceInterface(int scaling, sf::Font& font, sf::Texture& textureAtlas, json& atlasData, json& SmeltingRecipeData) :
   inputInventory(1, 1, scaling, 1, font, textureAtlas, atlasData),
   fuelInventory(1, 1, scaling, 1, font, textureAtlas, atlasData),
   outputInventory(1, 1, scaling, 1, font, textureAtlas, atlasData),
   smeltingRecipesData(smeltingRecipesData) {
    this->parseSmeltingRecipesData();
}

void FurnaceInterface::loadFromVector(std::vector<ItemStack> itemStacks) {
    this->inputInventory.setItemStack(0, itemStacks[0]);
    this->fuelInventory.setItemStack(0, itemStacks[1]);
    this->outputInventory.setItemStack(0, itemStacks[2]);
}

std::vector<ItemStack> FurnaceInterface::exportToVector() {
    std::vector<ItemStack> returnVal;
    returnVal.resize(3);
    returnVal[0] = this->inputInventory.getItemStack(0);
    returnVal[1] = this->fuelInventory.getItemStack(0);
    returnVal[2] = this->outputInventory.getItemStack(0);
    return returnVal;
}

void FurnaceInterface::setInputPosition(sf::Vector2f position) {
    this->inputInventory.setPosition(position);
}

void FurnaceInterface::setFuelPosition(sf::Vector2f position) {
    this->fuelInventory.setPosition(position);
}

void FurnaceInterface::setOutputPosition(sf::Vector2f position) {
    this->outputInventory.setPosition(position);
}

void FurnaceInterface::setScaling(int scaling) {
    this->inputInventory.setScaling(scaling);
    this->fuelInventory.setScaling(scaling);
    this->outputInventory.setScaling(scaling);
}

void FurnaceInterface::setProgess(float progress) {
    // TODO Implement mc_progressBar
}

void FurnaceInterface::parseSmeltingRecipesData() {
    size_t recipesCount = this->smeltingRecipesData.size();
    this->parsedSmeltingRecipesData.resize(recipesCount);
    for (size_t i = 0; i < recipesCount; i++) {
        this->parsedSmeltingRecipesData[i].ingredientID = this->smeltingRecipesData[i]["ingredient"];
        this->parsedSmeltingRecipesData[i].resultID = this->smeltingRecipesData[i]["result_item"];
    }
}

sf::FloatRect FurnaceInterface::getInputGlobalBounds() {
    return this->inputInventory.getGlobalBounds();
}

sf::FloatRect FurnaceInterface::getInputLocalBounds() {
    return this->inputInventory.getLocalBounds();
}

sf::FloatRect FurnaceInterface::getInputSlotGlobalBounds(int slotID) {
    return this->inputInventory.getSlotGlobalBounds(slotID);
}

sf::FloatRect FurnaceInterface::getInputSlotLocalBounds(int slotID) {
    return this->inputInventory.getSlotLocalBounds(slotID);
}

sf::FloatRect FurnaceInterface::getFuelGlobalBounds() {
    return this->fuelInventory.getGlobalBounds();
}

sf::FloatRect FurnaceInterface::getFuelLocalBounds() {
    return this->fuelInventory.getLocalBounds();
}

sf::FloatRect FurnaceInterface::getFuelSlotGlobalBounds(int slotID) {
    return this->fuelInventory.getSlotGlobalBounds(slotID);
}

sf::FloatRect FurnaceInterface::getFuelSlotLocalBounds(int slotID) {
    return this->fuelInventory.getSlotLocalBounds(slotID);
}

sf::FloatRect FurnaceInterface::getOutputGlobalBounds() {
    return this->outputInventory.getGlobalBounds();
}

sf::FloatRect FurnaceInterface::getOutputLocalBounds() {
    return this->outputInventory.getLocalBounds();
}

sf::FloatRect FurnaceInterface::getOutputSlotGlobalBounds(int slotID) {
    return this->outputInventory.getSlotGlobalBounds(slotID);
}

sf::FloatRect FurnaceInterface::getOutputSlotLocalBounds(int slotID) {
    return this->outputInventory.getSlotLocalBounds(slotID);
}

ItemStack FurnaceInterface::getInputItemStack(int slotID) {
    return this->inputInventory.getItemStack(slotID);
}

ItemStack FurnaceInterface::getFuelItemStack(int slotID) {
    return this->fuelInventory.getItemStack(slotID);
}

ItemStack FurnaceInterface::getOutputItemStack(int slotID) {
    return this->outputInventory.getItemStack(slotID);
}

int FurnaceInterface::getInputEmptySpace(int slotID) {
    return this->inputInventory.getEmptySpace(slotID);
}

int FurnaceInterface::getFuelEmptySpace(int slotID) {
    return this->fuelInventory.getEmptySpace(slotID);
}

int FurnaceInterface::subtractInputItem(int slotID, int amount) {
    return this->inputInventory.subtractItem(slotID, amount);
}

int FurnaceInterface::subtractFuelItem(int slotID, int amount) {
    return this->fuelInventory.subtractItem(slotID, amount);
}

int FurnaceInterface::subtractOutputItem(int slotID, int amount) {
    return this->outputInventory.subtractItem(slotID, amount);
}

void FurnaceInterface::setInputItemStack(int slotID, ItemStack itemStack) {
    this->inputInventory.setItemStack(slotID, itemStack);
}

void FurnaceInterface::setFuelItemStack(int slotID, ItemStack itemStack) {
    this->fuelInventory.setItemStack(slotID, itemStack);
}

ItemStack FurnaceInterface::addInputItemStack(int slotID, ItemStack itemStack) {
    return this->inputInventory.addItemStack(slotID, itemStack);
}

ItemStack FurnaceInterface::addFuelItemStack(int slotID, ItemStack itemStack) {
    return this->fuelInventory.addItemStack(slotID, itemStack);
}

ItemStack FurnaceInterface::takeOutputItem(int slotID) {
    ItemStack returnVal = this->outputInventory.getItemStack(slotID);
    this->outputInventory.setItemStack(slotID, ItemStack(0, 0));
    return returnVal;
}

}  // namespace mc