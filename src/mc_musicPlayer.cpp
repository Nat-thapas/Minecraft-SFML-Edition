#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>

#include "mc_musicPlayer.hpp"

namespace mc {

MusicPlayer::MusicPlayer(std::string musicFilesBasePath, std::vector<std::string> musicFileNames) {
    this->musicFilePaths.resize(musicFileNames.size());
    for (size_t i = 0; i < musicFileNames.size(); i++) {
        this->musicFilePaths[i] = musicFilesBasePath + musicFileNames[i];
    }
    this->shouldBePlaying = false;
}

void MusicPlayer::update() {
    if (!this->shouldBePlaying) {
        return;
    }
    if (this->music.getStatus() == sf::Music::Status::Playing) {
        return;
    }
    if (this->playing) {
        this->playing = false;
        this->clock.restart();
        waitTime = 10 + rand() % 21;
        return;
    }
    if (static_cast<int>(this->clock.getElapsedTime().asSeconds()) <= this->waitTime) {
        return;
    }

    int newMusicIdx;
    do {
        newMusicIdx = rand() % this->musicFilePaths.size();
    } while (newMusicIdx == this->currentMusicIdx);
    this->currentMusicIdx = newMusicIdx;
    this->music.openFromFile(this->musicFilePaths[this->currentMusicIdx]);
    this->music.play();
    this->playing = true;
}

void MusicPlayer::start() {
    this->currentMusicIdx = -1;
    this->playing = false;
    this->waitTime = 0;
    this->clock.restart();
    this->shouldBePlaying = true;
}

void MusicPlayer::stop() {
    this->music.stop();
    this->currentMusicIdx = -1;
    this->playing = false;
    this->waitTime = 0;
    this->clock.restart();
    this->shouldBePlaying = false;
}

}  // namespace mc