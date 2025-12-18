#include "game_state.h"
#include <ctime>

GameState::GameState(int num_players, time_t round_duration)
    : max_players(max_players),
      round_end_time(0),
      round_duration(round_duration),
      game_start_time(0),
      players_list(),
      game(std::nullopt) {}

bool GameState::add_player(const std::string& player_name) {
    // blokada duplikatów w lobby
    for (size_t i = 0; i < players_list.size(); ++i) {
        const Player& p = players_list[i];
        if (p.player_name == player_name) return false;
    }

    // jeśli gra trwa, to też blokujemy duplikaty z aktualnej rozgrywki
    if (game.has_value()) {
        for (const auto& p : game->players_list) {
            if (p.player_name == player_name) return false;
        }
    }

    // dodaj do lobby
    players_list.emplace_back(player_name);
    return true;
}

bool GameState::remove_player(const std::string& player_name) {
    // usuwamy TYLKO z lobby (poczekalni)
    for (auto it = players_list.begin(); it != players_list.end(); ++it) {
        if (it->player_name == player_name) {
            players_list.erase(it);
            return true;
        }
    }
    return false;
}

bool GameState::start_game() {
    // gra już trwa
    if (game.has_value()) return false;

    // start tylko gdy w lobby jest min 3 graczy
    if (players_list.size() < 3) return false;

    game_start_time = std::time(nullptr);
    round_end_time = game_start_time + round_duration;

    // przerzucamy lobby do gry (po starcie lobby ma być puste)
    game.emplace(std::move(players_list), round_duration);
    players_list.clear();

    // start pierwszej rundy
    game->start_round();

    return true;
}

void GameState::end_game() {
    // kończymy aktywną grę i przerzucamy graczy z gry do lobby (żeby mogli grać znowu)
    game = std::nullopt;


    round_end_time = 0;
    game_start_time = 0;
}

