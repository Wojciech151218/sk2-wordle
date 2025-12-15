// game_state.h

#pragma once

#include <vector>
#include <string>
#include <ctime>  // for time_t type

class GameState {
private:
    int num_players; // Liczba graczy
    int round; // Numer rundy
    time_t round_end_time; // Czas do końca rundy
    time_t round_duration; // Czas trwania jednej rundy
    time_t game_start_time; // Czas rozpoczęcia gry


    std::vector<Player> players_list; // Lista graczy
    std::optional<Game> games; // Bieżąca gra

public:
    GameState(int num_players, time_t round_duration);
    
    bool add_player(const std::string& player_name);    // Dodawanie gracza  //walidacja czy nazwa gracza juz istnieje w grze 
    bool remove_player(const std::string& player_name); // Usunięcie gracza

    bool start_game();  // Rozpoczęcie gry
    void end_game();    // Zakończenie gry
    

    bool is_game_active() const; // Sprawdzenie, czy gra trwa

};
