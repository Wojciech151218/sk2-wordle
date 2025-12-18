#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <optional>

#include "player.h"
#include "game.h"

/*
    - trzyma graczy w poczekalni (lobby)
    - startuje grę, gdy jest min. 3 graczy
    - nie zarządza rundami bezpośrednio (od tego jest Game)
*/
class GameState {
private:
    int num_players;        // ile osób jest w LOBBY 
    int round;              // numer rundy 
    time_t round_end_time;  // kiedy kończy się aktualna runda
    time_t round_duration;  // ile trwa jedna runda
    time_t game_start_time; // kiedy startuje gra

    // LOBBY
    std::vector<Player> players_list;

    // Aktywna gra 
    std::optional<Game> games;

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

    // Czy gra aktualnie działa
    bool is_game_active() const;
};
