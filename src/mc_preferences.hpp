#ifndef MC_PREFERENCES_HPP
#define MC_PREFERENCES_HPP

#include <SFML/System.hpp>

namespace mc {

struct PreferencesData {
    bool fullScreenEnabled;
    sf::Vector2i screenSize;
    int framerateLimit;
    bool vsyncEnabled;
    int antialiasingLevel;
    int uiScaling;
    int gamePixelPerBlock;
    int masterVolume;
    int musicVolume;
};

class Preferences {
   public:
    static PreferencesData getDefault();
    static PreferencesData loadFromFile(std::string filePath);
    static bool saveToFile(std::string filePath, PreferencesData preferencesData);
};

}  // namespace mc

#endif  // MC_PREFERENCES_HPP