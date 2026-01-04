#pragma once
#include <vector>
#include <string>
#include <ctime>
#include <optional>

#include "player.h"
#include "game.h"
#include "nlohmann/json.hpp"
#include "logic/endpoints/request_bodies.h"
#include "vote.h"


/*
    - trzyma graczy w poczekalni (lobby)
    - startuje grę, gdy jest min. 3 graczy
    - nie zarządza rundami bezpośrednio (od tego jest Game)
*/



class GameState {
private:
    time_t round_end_time;  // kiedy kończy się aktualna runda
    time_t round_duration;  // ile trwa jedna runda
    time_t game_start_time; // kiedy startuje gra

    // LOBBY
    std::vector<Player> players_list; 

    // Aktywna gra 
    std::optional<Game> game;

    std::optional<Vote> current_vote;

    // VOTE TIMER
    time_t vote_duration;  // ile trwa głosowanie (sekundy)
    time_t vote_end_time;  // kiedy kończy się głosowanie (unix time)
    
public:
    Result<GameState> set_ready(const StateRequest& request);
    bool all_ready_in_lobby() const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        GameState,
        round_end_time,
        round_duration,
        game_start_time,
        players_list,
        game,
        current_vote,
        vote_duration,
        vote_end_time
    )

    GameState(time_t round_duration);
    Result<GameState> vote(std::string voting_player, std::string voted_player, bool vote_for);

    void end_vote();

    // Dodaje gracza do lobby
    Result<GameState> add_player(const JoinRequest& request);

    // Usuwa gracza z lobby
    Result<GameState> remove_player(const JoinRequest& request);

    // Startuje grę
    bool start_game();

    // Kończy grę i resetuje stan (przerzuca graczy z powrotem do lobby)
    void end_game();
    void next_round();

    void game_tick(); // ta metoda bedzie gdzies wywolywana asychronicznie by zegar gry szedl do przodu

    Result<GameState> get_state(const StateRequest& request) const; //ta metoda zwraca stan gry w formacie json

    //Result<std::vector<WordleWord>> make_guess(const GuessRequest& request);//ta metoda przekazuje guess do aktualnej rundy
    Result<std::vector<WordleWord>> make_guess(const GuessRequest& request);




};
