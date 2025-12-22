#pragma once
#include <vector>
#include <string>
#include <ctime>

#include "player.h"
#include "round.h"

class GameState;

/*
    Game == logika rozgrywki
    - ma listę graczy, którzy aktualnie grają (nie lobby)
    - tworzy i przełącza rundy
    - przyjmuje guess od gracza i przekazuje do aktualnej rundy
*/
class Game {
private:
    time_t round_end_time;      // koniec aktualnej rundy
    time_t round_duration;      // ile trwa runda
    time_t game_start_time;     // kiedy wystartowała gra

    // gracze biorący udział w aktualnej grze
    std::vector<Player> players_list;

    // lista rund - ostatni element to aktywna runda
    std::vector<Round> rounds;


    //by przerzucić graczy z GameState do Game
    friend class GameState;

    // pomocniczo: znajdź gracza po nicku i zwróć wskaźnik na obiekt w players_list
    Player* find_player_ptr_by_name(const std::string& player_name);

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Game, round_end_time, round_duration, game_start_time, players_list, rounds)

    // Konstruktor gry
    Game(std::vector<Player> player, time_t round_duration);

    // Zakończenie rundy:
    // jeśli jeszcze ktoś żyje -> start następnej rundy
    bool end_round();

    // Start rundy:
    // tworzy nowy obiekt Round i wrzuca do rounds
    bool start_round();

    // Sprawdza czy gra się skończyła (<= 1 żywy gracz)
    bool check_if_game_is_over();

    int get_round() const;

    // Gracz wysyła guess: Game przekazuje to do aktualnej rundy
    Result<std::vector<WordleWord>> make_guess(const std::string& player_name,
                                          const std::string& guess,
                                          std::time_t client_ts);

};
