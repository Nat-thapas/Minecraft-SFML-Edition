#include <fstream>
#include <zlib.h>

#include "mc_preferences.hpp"

namespace mc {

PreferencesData Preferences::getDefault() {
    PreferencesData preferencesData;
    preferencesData.fullScreenEnabled = false;
    preferencesData.screenSize = sf::Vector2i(854, 480);
    preferencesData.framerateLimit = 120;
    preferencesData.vsyncEnabled = false;
    preferencesData.antialiasingLevel = 0;
    preferencesData.uiScaling = 2;
    preferencesData.gamePixelPerBlock = 32;
    preferencesData.masterVolume = 100;
    preferencesData.musicVolume = 50;
    return preferencesData;
}

PreferencesData Preferences::loadFromFile(std::string filePath) {
    PreferencesData preferencesData;
    gzFile inFileZ = gzopen(filePath.c_str(), "rb");
    gzread(inFileZ, reinterpret_cast<char*>(&preferencesData), sizeof(PreferencesData));
    gzclose(inFileZ);
    return preferencesData;
}

bool Preferences::saveToFile(std::string filePath, PreferencesData preferencesData) {
    gzFile outFileZ = gzopen(filePath.c_str(), "wb");
    gzwrite(outFileZ, reinterpret_cast<char*>(&preferencesData), sizeof(PreferencesData));
    gzclose(outFileZ);
    return true;
}

}  // namespace mc