#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

#include "../include/json.hpp"
#include "mc_inventory.hpp"

#include "mc_craftingInterface.hpp"

using json = nlohmann::json;

namespace mc {

void CraftingInterface::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->inputInventory, states);
    target.draw(this->outputInventory, states);
}

CraftingInterface::CraftingInterface(int size, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData, json& recipesData)
   : inputInventory(size*size, size, scaling, margin, font, textureAtlas, atlasData),
   outputInventory(1, 1, scaling, margin, font, textureAtlas, atlasData), 
   recipesData(recipesData) {
    this->size = size;
    this->parseRecipesData();
}

void CraftingInterface::setInputPosition(sf::Vector2f position) {
    this->inputInventory.setPosition(position);
}

void CraftingInterface::setOutputPosition(sf::Vector2f position) {
    this->outputInventory.setPosition(position);
}

void CraftingInterface::setScaling(int scaling) {
    this->inputInventory.setScaling(scaling);
    this->outputInventory.setScaling(scaling);
}

void CraftingInterface::setMargin(int margin) {
    this->inputInventory.setMargin(margin);
    this->outputInventory.setMargin(margin);
}

void CraftingInterface::parseRecipesData() {
    size_t recipesCount = this->recipesData.size();
    this->parsedRecipesData.resize(recipesCount);
    for (size_t i = 0; i < recipesCount; i++) {
        this->parsedRecipesData[i].width = this->recipesData[i]["width"];
        this->parsedRecipesData[i].height = this->recipesData[i]["height"];
        this->parsedRecipesData[i].shaped = this->recipesData[i]["shaped"];
        size_t ingredientsCount = this->recipesData[i]["ingredients"].size();
        this->parsedRecipesData[i].ingredients.resize(ingredientsCount);
        for (size_t j = 0; j < ingredientsCount; j++) {
            this->parsedRecipesData[i].ingredients[j] = this->recipesData[i]["ingredients"][j];
        }
        this->parsedRecipesData[i].resultItemStack = ItemStack(this->recipesData[i]["result_item"], this->recipesData[i]["result_amount"]);
    }
}

sf::IntRect CraftingInterface::getRecipeRect() {
    int minX = this->size;
    int minY = this->size;
    int maxX = -1;
    int maxY = -1;
    bool itemExist = false;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (this->getInputItemStack(x + y * size).id != 0) {
                itemExist = true;
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        }
    }
    if (itemExist) {
        return sf::IntRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
    }
    return sf::IntRect(0, 0, 0, 0);
}

std::vector<int> CraftingInterface::getMatchVector() {
    sf::IntRect recipeRect = this->getRecipeRect();
    std::vector<int> matchVector;
    matchVector.resize(recipeRect.width * recipeRect.height);
    for (int x = recipeRect.left; x < recipeRect.left + recipeRect.width; x++) {
        for (int y = recipeRect.top; y < recipeRect.top + recipeRect.height; y++) {
            matchVector[(x - recipeRect.left) + (y - recipeRect.top) * recipeRect.width] = this->getInputItemStack(x + y * this->size).id;
        }
    }
    return matchVector;
}

std::vector<int> CraftingInterface::getShapelessMatchVector(std::vector<int> matchVector) {
    int zerosCount = std::count(matchVector.cbegin(), matchVector.cend(), 0);
    std::sort(matchVector.begin(), matchVector.end());
    return std::vector<int>(matchVector.cbegin() + zerosCount, matchVector.cend());
}

void CraftingInterface::updateOutput() {
    // TODO support shapeless recipes
    sf::IntRect recipeRect = this->getRecipeRect();
    std::vector<int> matchVector = this->getMatchVector();
    for (size_t i = 0; i < this->parsedRecipesData.size(); i++) {
        if (matchVector == this->parsedRecipesData[i].ingredients && recipeRect.width == this->parsedRecipesData[i].width && recipeRect.height == this->parsedRecipesData[i].height) {
            this->setOutputItemStack(0, this->parsedRecipesData[i].resultItemStack);
            return;
        }
        if (this->parsedRecipesData[i].shaped == false && this->getShapelessMatchVector(matchVector) == this->parsedRecipesData[i].ingredients) {
            this->setOutputItemStack(0, this->parsedRecipesData[i].resultItemStack);
            return;
        }
    }
    this->setOutputItemStack(0, ItemStack(0, 0));
}

void CraftingInterface::setOutputItemStack(int slotID, ItemStack itemStack) {
    this->outputInventory.setItemStack(slotID, itemStack);
}

sf::FloatRect CraftingInterface::getInputGlobalBounds() {
    return this->inputInventory.getGlobalBounds();
}

sf::FloatRect CraftingInterface::getInputLocalBounds() {
    return this->inputInventory.getLocalBounds();
}

sf::FloatRect CraftingInterface::getInputSlotGlobalBounds(int slotID) {
    return this->inputInventory.getSlotGlobalBounds(slotID);
}

sf::FloatRect CraftingInterface::getInputSlotLocalBounds(int slotID) {
    return this->inputInventory.getSlotLocalBounds(slotID);
}

sf::FloatRect CraftingInterface::getOutputGlobalBounds() {
    return this->outputInventory.getGlobalBounds();
}

sf::FloatRect CraftingInterface::getOutputLocalBounds() {
    return this->outputInventory.getLocalBounds();
}

sf::FloatRect CraftingInterface::getOutputSlotGlobalBounds(int slotID) {
    return this->outputInventory.getSlotGlobalBounds(slotID);
}

sf::FloatRect CraftingInterface::getOutputSlotLocalBounds(int slotID) {
    return this->outputInventory.getSlotLocalBounds(slotID);
}

ItemStack CraftingInterface::getInputItemStack(int slotID) {
    return this->inputInventory.getItemStack(slotID);
}

ItemStack CraftingInterface::getOutputItemStack(int slotID) {
    return this->outputInventory.getItemStack(slotID);
}

int CraftingInterface::getInputEmptySpace(int slotID) {
    return this->inputInventory.getEmptySpace(slotID);
}

int CraftingInterface::subtractInputItem(int slotID, int amount) {
    return this->inputInventory.subtractItem(slotID, amount);
}

void CraftingInterface::setInputItemStack(int slotID, ItemStack itemStack) {
    this->inputInventory.setItemStack(slotID, itemStack);
    this->updateOutput();
}

ItemStack CraftingInterface::addInputItemStack(int slotID, ItemStack itemStack) {
    ItemStack returnVal = this->inputInventory.addItemStack(slotID, itemStack);
    this->updateOutput();
    return returnVal;
}

ItemStack CraftingInterface::takeOutputItem(int slotID) {
    if (slotID != 0) {
        return ItemStack(0, 0);
    }
    ItemStack returnVal = this->outputInventory.getItemStack(0);
    for (int i = 0; i < this->size * this->size; i++) {
        if (this->inputInventory.getItemStack(i).amount > 1) {
            this->inputInventory.setItemStack(i, ItemStack(this->inputInventory.getItemStack(i).id, this->inputInventory.getItemStack(i).amount - 1));
        } else {
            this->inputInventory.setItemStack(i, ItemStack(0, 0));
        }
    }
    this->updateOutput();
    return returnVal;
}
 
}  //namespace mc