#include "mc_inventory.hpp"

#include <SFML/Graphics.hpp>
#include <format>

#include "../include/json.hpp"
#include "idiv.hpp"
#include "mod.hpp"

using json = nlohmann::json;

namespace mc {

void Inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &this->textureAtlas;
    target.draw(this->vertexArray, states);
    for (int i = 0; i < this->size; i++) {
        if (this->itemStacks[i].amount > 1) {
            target.draw(this->amountLabels[i], states);
        }
    }
}

Inventory::Inventory(int size, int width, int scaling, int margin, sf::Font& font, sf::Texture& textureAtlas, json& atlasData) : font(font), textureAtlas(textureAtlas), atlasData(atlasData) {
    this->size = size;
    this->width = width;
    this->scaling = scaling;
    this->margin = margin;
    this->itemStacks.resize(size);
    this->amountLabels.resize(size);
    for (int i = 0; i < size; i++) {
        this->itemStacks[i] = ItemStack(0, 0);
        this->amountLabels[i] = sf::Text();
    }
    this->vertexArray.setPrimitiveType(sf::Triangles);
    this->vertexArray.resize(size * 6);
    this->parseAtlasData();
    this->initializeVertexArray();
    this->initializeAmountLabels();
    this->updateAllVertexArray();
    this->updateAllAmountLabels();
}

void Inventory::parseAtlasData() {
    sf::IntRect texRect;
    for (int blockID = 0; blockID < 71; blockID++) {
        texRect.left = this->atlasData[std::format("{:03d}", blockID)]["x"];
        texRect.top = this->atlasData[std::format("{:03d}", blockID)]["y"];
        texRect.width = this->atlasData[std::format("{:03d}", blockID)]["w"];
        texRect.height = this->atlasData[std::format("{:03d}", blockID)]["h"];
        this->parsedAtlasData[blockID] = texRect;
    }
}

void Inventory::initializeVertexArray() {
    sf::IntRect itemRect;

    itemRect.width = this->scaling * 16;
    itemRect.height = this->scaling * 16;

    for (int i = 0; i < this->size; i++) {
        itemRect.left = (i % this->width) * (itemRect.width + this->margin * this->scaling * 2) + this->margin * this->scaling;
        itemRect.top = (i / this->width) * (itemRect.height + this->margin * this->scaling * 2) + this->margin * this->scaling;

        this->vertexArray[i * 6].position = sf::Vector2f(itemRect.left, itemRect.top);
        this->vertexArray[i * 6 + 1].position = sf::Vector2f(itemRect.left + itemRect.width, itemRect.top);
        this->vertexArray[i * 6 + 2].position = sf::Vector2f(itemRect.left, itemRect.top + itemRect.height);
        this->vertexArray[i * 6 + 3].position = sf::Vector2f(itemRect.left, itemRect.top + itemRect.height);
        this->vertexArray[i * 6 + 4].position = sf::Vector2f(itemRect.left + itemRect.width, itemRect.top);
        this->vertexArray[i * 6 + 5].position = sf::Vector2f(itemRect.left + itemRect.width, itemRect.top + itemRect.height);
    }
}

void Inventory::updateAllVertexArray() {
    int itemID;
    sf::IntRect textureRect;

    for (int i = 0; i < this->size; i++) {
        itemID = this->itemStacks[i].id;
        textureRect = this->parsedAtlasData[itemID];

        this->vertexArray[i * 6].texCoords = sf::Vector2f(textureRect.left, textureRect.top);
        this->vertexArray[i * 6 + 1].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 2].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 3].texCoords = sf::Vector2f(textureRect.left, textureRect.top + textureRect.height);
        this->vertexArray[i * 6 + 4].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top);
        this->vertexArray[i * 6 + 5].texCoords = sf::Vector2f(textureRect.left + textureRect.width, textureRect.top + textureRect.height);
    }
}

void Inventory::initializeAmountLabels() {
    for (int i = 0; i < this->size; i++) {
        this->amountLabels[i].setPosition(sf::Vector2f(this->getSlotLocalBounds(i).left + static_cast<float>(this->scaling * 16 / 3), this->getSlotLocalBounds(i).top + static_cast<float>(this->scaling * 16 / 3)));
        this->amountLabels[i].setFont(this->font);
        this->amountLabels[i].setCharacterSize(this->scaling * 10);
        this->amountLabels[i].setFillColor(sf::Color::White);
        this->amountLabels[i].setOutlineColor(sf::Color::Black);
        this->amountLabels[i].setOutlineThickness(0.5f * this->scaling);
    }
}

void Inventory::updateAllAmountLabels() {
    for (int i = 0; i < this->size; i++) {
        this->amountLabels[i].setString(std::format("{:2d}", this->itemStacks[i].amount));
    }
}

void Inventory::setScaling(int scaling) {
    if (this->scaling == scaling) {
        return;
    }
    this->scaling = scaling;
    this->initializeVertexArray();
    this->initializeAmountLabels();
    this->updateAllVertexArray();
    this->updateAllAmountLabels();
}

sf::FloatRect Inventory::getGlobalBounds() {
    sf::FloatRect globalBounds;
    globalBounds.left = this->getPosition().x;
    globalBounds.top = this->getPosition().y;
    globalBounds.width = static_cast<float>(this->size % this->width * (this->scaling * (16 + this->margin * 2)) + this->scaling * this->margin); 
    globalBounds.height = static_cast<float>((this->size / this->width + (this->size % this->width > 0)) * (this->scaling * (16 + this->margin * 2)) + this->scaling * this->margin);
    return globalBounds;
}

sf::FloatRect Inventory::getLocalBounds() {
    sf::FloatRect localBounds;
    localBounds.left = 0.f;
    localBounds.top = 0.f;
    localBounds.width = static_cast<float>(std::min(this->width, this->size) * (this->scaling * (16 + this->margin * 2)) + this->scaling * this->margin); 
    localBounds.height = static_cast<float>((this->size / this->width + (this->size % this->width > 0)) * (this->scaling * (16 + this->margin * 2)) + this->scaling * this->margin);
    return localBounds;
}

sf::FloatRect Inventory::getSlotGlobalBounds(int slotID) {
    sf::FloatRect globalBounds;
    globalBounds.left = this->getPosition().x + static_cast<float>((slotID % this->width) * (this->scaling * (16 + this->margin * 2)) + this->margin * this->scaling);
    globalBounds.top = this->getPosition().y + static_cast<float>((slotID / this->width) * (this->scaling * (16 + this->margin * 2)) + this->margin * this->scaling);
    globalBounds.width = static_cast<float>(this->scaling * (16 + this->margin * 2));
    globalBounds.height = static_cast<float>(this->scaling * (16 + this->margin * 2));
    return globalBounds;
}


sf::FloatRect Inventory::getSlotLocalBounds(int slotID) {
    sf::FloatRect localBounds;
    localBounds.left = static_cast<float>((slotID % this->width) * (this->scaling * (16 + this->margin * 2)) + this->margin * this->scaling);
    localBounds.top = static_cast<float>((slotID / this->width) * (this->scaling * (16 + this->margin * 2)) + this->margin * this->scaling);
    localBounds.width = static_cast<float>(this->scaling * (16 + this->margin * 2));
    localBounds.height = static_cast<float>(this->scaling * (16 + this->margin * 2));
    return localBounds;
}

ItemStack Inventory::getItemStack(int slotID) {
    if (slotID < 0 || slotID >= this->size) return ItemStack(0, 0);
    return this->itemStacks[slotID];
}

void Inventory::setItemStack(int slotID, ItemStack itemStack) {
    if (slotID < 0 || slotID >= this->size) return;
    this->itemStacks[slotID] = itemStack;
}

ItemStack Inventory::addItemStack(ItemStack itemStack) {
    for (int i = 0; i < size && itemStack.amount > 0; i++) {
        if (this->itemStacks[i].id == itemStack.id && this->stackSizes[itemStack.id] - this->itemStacks[i].amount > 0) {
            int emptySpace = this->stackSizes[itemStack.id] - this->itemStacks[i].amount;
            this->itemStacks[i].amount += std::min(itemStack.amount, emptySpace);
            itemStack.amount -= std::min(itemStack.amount, emptySpace);
        }
    }
    for (int i = 0; i < size && itemStack.amount > 0; i++) {
        if (this->itemStacks[i].id == 0) {
            int emptySpace = this->stackSizes[itemStack.id];
            this->itemStacks[i].id = itemStack.id;
            this->itemStacks[i].amount += std::min(itemStack.amount, emptySpace);
            itemStack.amount -= std::min(itemStack.amount, emptySpace);
        }
    }
    if (itemStack.amount == 0) {
        return ItemStack(0, 0);
    }
    this->updateAllVertexArray();
    this->updateAllAmountLabels();
    return itemStack;
}

int Inventory::subtractItem(int slotID, int amount) {
    int subtractAmount = std::min(this->itemStacks[slotID].amount, amount);
    this->itemStacks[slotID].amount -= subtractAmount;
    if (this->itemStacks[slotID].amount <= 0) {
        this->itemStacks[slotID].id = 0;
        this->updateAllVertexArray();
    }
    this->updateAllAmountLabels();
    return amount - subtractAmount;
}

}  // namespace mc