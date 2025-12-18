#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <optional>

#include "player.h"
#include "game.h"
#include "nlohmann/json.hpp"

/*
    - trzyma graczy w poczekalni (lobby)
    - startuje grę, gdy jest min. 3 graczy
    - nie zarządza rundami bezpośrednio (od tego jest Game)
*/
class GameState {
private:
    int max_players;        // maksymalna liczba graczy w LOBBY 
    time_t round_end_time;  // kiedy kończy się aktualna runda
    time_t round_duration;  // ile trwa jedna runda
    time_t game_start_time; // kiedy startuje gra

    // LOBBY
    std::vector<Player> players_list; 

    // Aktywna gra 
    std::optional<Game> game;

public:
    GameState(int num_players, time_t round_duration);

    // Dodaje gracza do lobby
    bool add_player(const std::string& player_name);

    // Usuwa gracza z lobby
    bool remove_player(const std::string& player_name);

    // Startuje grę
    bool start_game();

    // Kończy grę i resetuje stan (przerzuca graczy z powrotem do lobby)
    void end_game();

    void game_tick(); // ta metoda bedzie gdzies wywolywana asychronicznie by zegar gry szedl do przodu

    nlohmann::json get_state() const; //ta metoda zwraca stan gry w formacie json

    void guess(std::string player_name, std::string guess);//ta metoda przekazuje guess do aktualnej rundy


};
