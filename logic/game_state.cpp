#include "game_state.h"
#include <ctime>
#include "server/cron/cron.h"

GameState::GameState(time_t round_duration)
    : round_end_time(0),
      round_duration(round_duration),
      game_start_time(0),
      players_list(),
      game(std::nullopt) {}

Result<GameState> GameState::add_player(const JoinRequest& request) {
    std::string player_name = request.player_name;

    //todo
    // blokada duplikatów w lobby
    // for (size_t i = 0; i < players_list.size(); ++i) {
    //     const Player& p = players_list[i];
    //     if (p.player_name == player_name) 
    //         return Error("Player already in lobby", HttpStatusCode::FORBIDDEN);
    // }

    // // jeśli gra trwa, to też blokujemy duplikaty z aktualnej rozgrywki
    // if (game.has_value()) {
    //     for (const auto& p : game->players_list) {
    //         if (p.player_name == player_name) 
    //             return Error("Player already in game", HttpStatusCode::FORBIDDEN);
    //     }
    // }
    // players_list.emplace_back(player_name);


    // dodaj do lobby

    if(std::find_if(players_list.begin(), players_list.end(), 
    [&](const Player& p){ return p.player_name == player_name; }) == players_list.end()) {
        players_list.push_back(player_name);
    }

    return Result<GameState>(*this);
}



Result<GameState> GameState::remove_player(const JoinRequest& request) {
    std::string player_name = request.player_name;
    // usuwamy tylko z lobby (poczekalni)
    for (auto it = players_list.begin(); it != players_list.end(); ++it) {
        if (it->player_name == player_name) {
            players_list.erase(it);
            return Result<GameState>(*this);
        }
    }
    return Error("Player not found", HttpStatusCode::NOT_FOUND);
}

void GameState::next_round() {
    if (!game.has_value()) return;

    const bool next_started = game->end_round();
    if (!next_started) {
        end_game();   // <- PRZERZUT DO LOBBY
        return;
    }

    // (opcjonalnie) aktualizacja czasu rundy w GameState
    round_end_time = std::time(nullptr) + round_duration;
}


bool GameState::start_game() {
    // gra już trwa
    if (game.has_value()) return false;

    //start tylko gdy w lobby jest min 3 graczy
    if (players_list.size() < 1) return false;

    // wszyscy muszą być ready
    if (!all_ready_in_lobby()) return false;

    game_start_time = std::time(nullptr);
    round_end_time = game_start_time + round_duration;

    // przerzucamy lobby do gry (po starcie lobby ma być puste)
    game = Game(std::move(players_list), round_duration);
    players_list.clear();

    // start pierwszej rundyS

    
    game->start_round();

    return true;
}

void GameState::end_game() {
    if (!game.has_value()) return;

    // przerzuć graczy z gry do lobby
    players_list = std::move(game->players_list);

    // reset stanu graczy żeby mogli grać od nowa
    for (auto& p : players_list) {
        p.reset_state();
    }

    // wyczyść grę
    game.reset();

    // wyzeruj czasy w GameState
    round_end_time = 0;
    game_start_time = 0;
}



Result<GameState> GameState::get_state(const StateRequest& request) const {
    std::string player_name = request.player_name;
    std::time_t timestamp = request.timestamp;

    return Result<GameState>(*this);
}

Result<std::vector<WordleWord>> GameState::make_guess(const GuessRequest& request) {
    if (!game.has_value())
        return Error("Game not found", HttpStatusCode::NOT_FOUND);

    return game->make_guess(request.player_name, request.guess, request.timestamp);
}



bool GameState::all_ready_in_lobby() const {
    if (players_list.empty()) return false;

    for (size_t i = 0; i < players_list.size(); ++i) {
        const Player& p = players_list[i];
        if (!p.is_ready) return false; // GameState jest friend Player, więc może czytać private
    }
    return true;
}

Result<GameState> GameState::set_ready(const StateRequest& request) {
    const std::string player_name = request.player_name;

    // jak gra już trwa, to READY w lobby nie ma sensu (albo zwróć "already started")
    if (game.has_value()) {
        return Error("Game already started", HttpStatusCode::FORBIDDEN);
    }

    // znajdź gracza w lobby i ustaw ready
    for (size_t i = 0; i < players_list.size(); ++i) {
        Player& p = players_list[i];
        if (p.player_name == player_name) {
            p.set_is_ready(true);

            // jeśli wszyscy gotowi i jest min. liczba graczy -> start gry
            const size_t MIN_PLAYERS = 3;
            if (players_list.size() >= MIN_PLAYERS && all_ready_in_lobby()) {
                start_game();
            }

            return Result<GameState>(*this);
        }
    }

    return Error("Player not found in lobby", HttpStatusCode::NOT_FOUND);
}
