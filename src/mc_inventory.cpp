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
    for (const sf::Text& label : this->amountLabels) {
        target.draw(label, states);
    }
}

Inventory::Inventory(int size, int width, int scaling, sf::Font& font, sf::Texture& textureAtlas, json& atlasData) : font(font), textureAtlas(textureAtlas), atlasData(atlasData) {
    this->size = size;
    this->width = width;
    this->scaling = scaling;
    this->itemStacks.resize(size);
    this->amountLabels.resize(size);
    for (int i = 0; i < size; i++) {
        this->itemStacks[i] = ItemStack(64 - i, i);
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
        itemRect.left = (i % this->width) * (itemRect.width + 2) + 1;
        itemRect.top = (i / this->width) * (itemRect.height + 2) + 1;

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
        this->amountLabels[i].setPosition(sf::Vector2f(static_cast<float>((i % this->width) * ((this->scaling * 16) + 2) + 1 + (this->scaling * 8)), static_cast<float>((i / this->width) * ((this->scaling * 16) + 2) + 1 + (this->scaling * 8))));
        this->amountLabels[i].setFont(this->font);
        this->amountLabels[i].setCharacterSize(this->scaling * 8);
        this->amountLabels[i].setFillColor(sf::Color::White);
        this->amountLabels[i].setOutlineColor(sf::Color::Black);
        this->amountLabels[i].setOutlineThickness(0.25f * this->scaling);
    }
}

void Inventory::updateAllAmountLabels() {
    for (int i = 0; i < this->size; i++) {
        this->amountLabels[i].setString(std::format("{}", this->itemStacks[i].amount));
    }
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
    return itemStack;
}

}  // namespace mc