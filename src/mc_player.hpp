#ifndef MC_PLAYER_HPP
#define MC_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <string>

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
    sf::Vector2f acceleration;
    sf::Vector2f speed;
    int chunkID;
    sf::Vector2f position;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

   public:
    int xp;

    Player(Chunks& chunks, int chunkID, sf::Vector2f pos, sf::Vector2i screenSize, int pixelPerBlock, std::string textureFilePath, float movementForce, float mass, float gravity, float frictionCoefficient);
};

}  // namespace mc

#endif  // MC_PLAYER_HPP