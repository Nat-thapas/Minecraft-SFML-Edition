#ifndef MC_PLAYER_HPP
#define MC_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>

#include "mc_chunks.hpp"
#include "idiv.hpp"
#include "mod.hpp"

namespace mc {

class Player : public sf::Drawable {
    Chunks& chunks;
    sf::Vector2i screenSize;
    int pixelPerBlock;
    sf::Texture texture;
    sf::Sprite sprite;
    float movementForce;
    float mass;
    float gravity;
    float frictionCoefficient;
    float airDragCoefficient;
    sf::Vector2f acceleration;
    sf::Vector2f velocity;
    int chunkID;
    sf::Vector2f position;
    std::array<bool, 71> blockSolidity = {0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool isOnGround();
    int getRelativeBlock(sf::Vector2f delta);
    int getRelativeChunkBlock(sf::Vector2f position);
    int getAbsoluteBlock(int chunkID, sf::Vector2f position);
    bool isBlockSolid(int blockID);

   public:
    int xp;

    Player(Chunks& chunks, int chunkID, sf::Vector2f position, sf::Vector2i screenSize, int pixelPerBlock, std::string textureFilePath, float movementForce, float mass, float gravity, float frictionCoefficient, float airDragCoefficient);
    void setScreenSize(sf::Vector2i screenSize);
    void setPixelPerBlock(int pixelPerBlock);
    int getChunkID();
    sf::Vector2f getPosition();
    void setLateralForce(int force);
    void jump();
    void update(sf::Time frameTime);
};

}  // namespace mc

#endif  // MC_PLAYER_HPP