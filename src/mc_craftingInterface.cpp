#include <SFML/Graphics.hpp>

#include "../include/json.hpp"
#include "mc_inventory.hpp"

#include "mc_craftingInterface.hpp"

using json = nlohmann::json;

namespace mc {

void CraftingInterface::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(this->inputInventory, states);
    target.draw(this->outputInventory, states);
}

CraftingInterface::CraftingInterface(int size, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData)
   : inputInventory(size*size, size, scaling, margin, font, textureAtlas, atlasData),
   outputInventory(1, 1, scaling, margin, font, textureAtlas, atlasData) {

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

void CraftingInterface::setInputItemStack(int slotID, ItemStack itemStack) {
    this->inputInventory.setItemStack(slotID, itemStack);
}

ItemStack CraftingInterface::addInputItemStack(int slotID, ItemStack itemStack) {
    return this->inputInventory.addItemStack(slotID, itemStack);
}

int CraftingInterface::subtractOutputItem(int slotID, int amount) {
    // TODO
    return 0;
}
 
}  //namespace mc