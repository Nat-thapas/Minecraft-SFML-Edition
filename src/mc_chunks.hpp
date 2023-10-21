#ifndef MC_CHUNKS_HPP
#define MC_CHUNKS_HPP

#include <SFML/Graphics.hpp>
#include <array>
#include <queue>
#include <string>
#include <unordered_map>

#include "../include/json.hpp"
#include "../include/perlin.hpp"
#include "mc_soundEffect.hpp"

#include "mc_chunk.hpp"

using json = nlohmann::json;
using Perlin = siv::PerlinNoise;

namespace mc {

class Chunks : public sf::Drawable {
    int seed;
    std::string worldName;
    std::string atlasFilesPath;
    std::string atlasDatasPath;
    std::array<sf::IntRect, 71> parsedAtlasData;
    json& smeltingRecipesData;
    std::unordered_map<int, int> parsedSmeltingRecipesData;
    sf::Texture textureAtlas;
    sf::Shader chunkShader;
    json atlasData;
    std::deque<mc::Chunk> chunks;
    Perlin noise;
    int pixelPerBlock;
    sf::Vector2i screenSize;
    int chunkCountOnScreen;
    int chunksStartID;
    int chunksEndID;
    int playerChunkID;
    sf::Vector2f playerPos;
    sf::Vector2i mouseScreenPos;
    int mouseChunkID;
    sf::Vector2i mousePos;
    sf::RectangleShape highlighter;
    int breakingChunkID;
    sf::Vector2i breakingPos;
    float breakProgress;
    sf::Sprite breakProgressOverlay;
    sf::Texture breakProgressOverlayTexture;
    sf::Shader overlayShader;
    SoundEffect& soundEffect;
    int lastBreakingSoundPlayedTickCount;
    std::array<bool, 71> isBlockBreakable = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::array<int, 123> breakSpeedMultipliers = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 4, 4, 4, 1, 6, 6, 6, 1, 12, 12, 12, 1, 8, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::unordered_map<int, int> toolsBreakGroup;
    std::unordered_map<int, int> toolsMiningLevel;
    std::array<int, 71> blocksBreakGroup = {0, 2, 1, 1, 2, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 3, 3, 0, 0, 0, 2, 2, 2, 2, 2, 1, 2, 0, 0, 3, 3, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 0, 2, 2, 0, 0, 0, 2, 0};
    std::array<int, 71> blocksMiningLevel = {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 2, 3, 3, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 1, 0, 4, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 255, 1, 1, 255, 255, 255, 1, 0};
    std::array<float, 71> blocksHardness = {0, 1.5, 0.6, 0.5, 2, 0, 2, 0.2, 2, 0, 65536, 0, 0, 0, 0, 0.5, 0.8, 0.8, 0.8, 0.6, 5, 5, 5, 5, 0.3, 0.2, 0.2, 0.8, 0, 0, 5, 5, 3, 5, 2, 0.6, 50, 0, 0, 2.5, 2.5, 3.5, 3.5, 0, 0, 0, 0, 0, 0, 0, 0, 0.6, 0.6, 1, 3, 3, 3, 3, 1.5, 1.5, 1.5, 1.5, 5, 65536, 0.4, 2, 65536, 65536, 65536, 3, 25};
    std::array<std::string, 71> blocksBreakingSound = {"", "step.stone", "step.grass", "step.gravel", "step.stone", "step.grass", "step.wood", "step.grass", "step.wood", "step.grass", "step.stone", "", "", "", "", "step.sand", "step.stone", "step.stone", "step.stone", "step.gravel", "step.stone", "step.stone", "step.stone", "step.stone", "step.stone", "step.wood", "step.wood", "step.cloth", "step.grass", "step.grass", "step.stone", "step.stone", "step.stone", "step.stone", "step.stone", "step.gravel", "step.stone", "step.wood", "", "step.wood", "step.wood", "step.stone", "step.stone", "step.grass", "step.grass", "step.grass", "step.grass", "step.grass", "step.grass", "step.grass", "step.grass", "step.gravel", "step.gravel", "step.wood", "step.wood", "step.wood", "step.wood", "step.wood", "step.stone", "step.stone", "step.stone", "step.stone", "step.stone", "step.glass", "step.stone", "step.stone", "step.glass", "step.stone", "step.stone", "step.stone", "step.stone"};

    void updateTexture();
    void parseAtlasData();
    void parseSmeltingRecipesData();
    void updateChunksPosition();
    void initializeChunks();
    void updateMousePosition();
    void updateHighlighterPosition();
    void updateBreakOverlayPosition();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    Chunks(int playerChunkID, int seed, int pixelPerBlock, std::string worldName, sf::Vector2i screenSize, std::string atlasFilesPath, std::string atlasDatasPath, std::string chunkShaderFilePath, std::string overlayShaderFilePath, json& smeltingRecipesData, std::string breakProgressOverlayTextureFilePath, SoundEffect& soundEffect);
    void tick(int tickCount);
    void update();
    void updateVertexArrays();
    void updateAnimatedVertexArrays();
    void updateLightLevels();
    void updateLightingVertexArray();
    void setPixelPerBlock(int pixelPerBlock);
    void setScreenSize(sf::Vector2i screenSize);
    int getPlayerChunkID();
    void setPlayerChunkID(int chunkID);
    sf::Vector2f getPlayerPos();
    void setPlayerPos(sf::Vector2f pos);
    void setMouseScreenPos(sf::Vector2i pos);
    int getMouseChunkID();
    sf::Vector2i getMousePos();
    sf::Vector2i getLoadedChunks();
    sf::Vector2i getPlayerLightLevel();
    sf::Vector2i getMouseLightLevel();
    int getBlock(int chunkID, int x, int y);
    void setBlock(int chunkID, int x, int y, int blockID);
    std::vector<ItemStack> breakBlock(int itemID, int tickCount);
    bool placeBlock(int itemID);
    Chunk& getChunk(int chunkID);
    bool saveAll();
};

}  // namespace mc

#endif  // MC_CHUNKS_HPP