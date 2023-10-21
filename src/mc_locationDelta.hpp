#ifndef MC_LOCATIONDELTA_HPP
#define MC_LOCATIONDELTA_HPP

#include <SFML/System.hpp>

namespace mc {

inline sf::Vector2f getLocationDelta(int chunkID1, sf::Vector2f pos1, int chunkID2, sf::Vector2f pos2) {
    sf::Vector2f delta = pos2 - pos1;
    delta.x += static_cast<float>(chunkID2 - chunkID1) * 16.f;
    delta.y += 2.f;
    return delta;
}

}  // namespace mc

#endif  // MC_LOCATIONDELTA_HPP