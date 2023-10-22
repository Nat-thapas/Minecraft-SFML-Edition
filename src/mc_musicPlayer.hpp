#ifndef MC_MUSICPLAYER_HPP
#define MC_MUSICPLAYER_HPP

#include <SFML/Audio.hpp>
#include <string>
#include <vector>

namespace mc {

class MusicPlayer {
    std::vector<std::string> musicFilePaths;
    sf::Music music;
    int currentMusicIdx;
    sf::Clock clock;
    bool playing;
    bool shouldBePlaying;
    int waitTime;

   public:
    MusicPlayer(std::string musicFilesBasePath, std::vector<std::string> musicFileNames, float volume);
    void update();
    void start();
    void stop();
    void setVolume(float volume);
};

}  // namespace mc

#endif  // MC_MUSICPLAYER_HPP