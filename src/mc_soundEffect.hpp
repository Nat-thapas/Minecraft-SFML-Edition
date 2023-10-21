#ifndef MC_SOUNDEFFECT_HPP
#define MC_SOUNDEFFECT_HPP

#include <SFML/Audio.hpp>
#include <array>
#include <string>
#include <unordered_map>

#include "../include/json.hpp"

using json = nlohmann::json;

namespace mc {

class SoundEffect {
    std::unordered_map<std::string, int> soundsVariantCount;
    std::unordered_map<std::string, sf::SoundBuffer> soundsBuffer;
    std::array<sf::Sound, 240> soundPlayers;
    int playerIdx;
    json soundData;

   public:
    SoundEffect(std::string soundsFolderPath);
    void play(std::string soundName, float volume = 1.f, sf::Vector2f position = sf::Vector2f(0.f, 0.f), float zDistance = 10.f);
};

}  // namespace mc

#endif  // MC_SOUNDEFFECT_HPP