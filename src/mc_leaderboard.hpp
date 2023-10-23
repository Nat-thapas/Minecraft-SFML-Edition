#ifndef MC_LEADERBOARD_HPP
#define MC_LEADERBOARD_HPP

#include <SFML/System.hpp>

namespace mc {

struct PlayerAndTime {
    int playtime;
    sf::String playerName;

    bool operator<(const PlayerAndTime& a) const {
        return playtime < a.playtime;
    }
};

}  // namespace mc

#endif  // MC_LEADERBOARD_HPP