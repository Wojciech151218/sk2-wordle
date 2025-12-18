#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>

#include "player.h"
#include "guesses.h"

/*
    Round == jedna konkretna runda:
*/
class Round {
private:
    std::string word;

    time_t round_end_time;
    time_t round_duration;
    time_t round_start_time;

    // Każdy gracz w tej rundzie ma swój obiekt Guesses
    std::unordered_map<Player*, Guesses> players_map;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Round, word, round_end_time, round_duration, round_start_time);

    Round(std::vector<Player*> player_list, time_t round_duration);

    // czy runda jeszcze trwa czasowo
    bool is_round_active();

    // gracz zgaduje:
    // - jeśli guess był błędny => round_errors++
    Result<Round> make_guess(Player* player, std::string& guess);
};
