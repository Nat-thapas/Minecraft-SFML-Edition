#include <SFML/Audio.hpp>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include "mc_soundEffect.hpp"

namespace mc {

SoundEffect::SoundEffect(std::string soundsFolderPath)  {
    std::ifstream soundDataFile(soundsFolderPath + "data.json");
    this->soundData = json::parse(soundDataFile);
    soundDataFile.close();
    for (size_t i = 0; i < this->soundData.size(); i++) {
        int variations = this->soundData[i]["variations"];
        std::string name = this->soundData[i]["name"];
        this->soundsVariantCount[name] = variations;
        if (variations == 1) {
            sf::SoundBuffer buffer;
            buffer.loadFromFile(soundsFolderPath + name + ".ogg");
            this->soundsBuffer[name] = buffer;
        } else {
            for (int v = 1; v <= variations; v++) {
                sf::SoundBuffer buffer;
                buffer.loadFromFile(soundsFolderPath + name + '.' + std::to_string(v) + ".ogg");
                this->soundsBuffer[name + '.' + std::to_string(v)] = buffer;
            }
        }
    }
    playerIdx = 0;
}

void SoundEffect::play(std::string soundName) {
    if (!this->soundsVariantCount.contains(soundName)) {
        std::cerr << "Sound: " << soundName << " doesn't exist" << std::endl;
        return;
    }
    int variations = this->soundsVariantCount[soundName];
    this->soundPlayers[this->playerIdx].stop();
    if (variations == 1) {
        this->soundPlayers[this->playerIdx].setBuffer(this->soundsBuffer[soundName]);
    } else {
        int playVariant = (rand() % variations) + 1;
        this->soundPlayers[this->playerIdx].setBuffer(this->soundsBuffer[soundName + '.' + std::to_string(playVariant)]);
    }
    this->soundPlayers[this->playerIdx].play();
    this->playerIdx++;
    this->playerIdx %= 240;
}

}  // namespace mc