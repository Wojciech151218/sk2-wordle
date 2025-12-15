// game_state.h

#pragma once

#include <vector>
#include <string>
#include <ctime>  

class Round {
private:
    std::string word;

    time_t round_end_time;      // Czas do końca rundy
    time_t round_duration;      // Czas trwania jednej rundy
    time_t game_start_time;     // Czas rozpoczęcia gry

    std::ordered_map<Player*, Guesses> players_map; // Mapa graczy
p

public:
    Round(std::vector<Player*> player_list, int num_players, time_t round_duration);
    
    bool is_round_active(); // const; // Sprawdzenie, czy runda trwa
    void make_guess(Player* player, std::string& guess); // Dodanie zgadywanego słowa przez gracza (pobranie hasla od klienta)

};
