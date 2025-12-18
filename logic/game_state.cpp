#include "game_state.h"
#include <ctime>

GameState::GameState(int num_players, time_t round_duration)
    : num_players(num_players),
      round(0),
      round_end_time(0),
      round_duration(round_duration),
      game_start_time(0),
      players_list(),
      games(std::nullopt) {}

bool GameState::add_player(const std::string& player_name) {
    // blokada duplikatów w lobby
    for (size_t i = 0; i < players_list.size(); ++i) {
        const Player& p = players_list[i];
        if (p.player_name == player_name) return false;
    }

    // jeśli gra trwa, to też blokujemy duplikaty z aktualnej rozgrywki
    if (games.has_value()) {
        for (const auto& p : games->players_list) {
            if (p.player_name == player_name) return false;
        }
    }

    // dodaj do lobby
    players_list.emplace_back(player_name);
    num_players = static_cast<int>(players_list.size());
    return true;
}

bool GameState::remove_player(const std::string& player_name) {
    // usuwamy TYLKO z lobby (poczekalni)
    for (auto it = players_list.begin(); it != players_list.end(); ++it) {
        if (it->player_name == player_name) {
            players_list.erase(it);
            num_players = static_cast<int>(players_list.size());
            return true;
        }
    }
    return false;
}

bool GameState::start_game() {
    // gra już trwa
    if (games.has_value() && games->is_game_active()) return false;

    // start tylko gdy w lobby jest min 3 graczy
    if (players_list.size() < 3) return false;

    game_start_time = std::time(nullptr);
    round_end_time = game_start_time + round_duration;
    round = 0;

    // przerzucamy lobby do gry (po starcie lobby ma być puste)
    games.emplace(std::move(players_list), round_duration);
    players_list.clear();
    num_players = 0;

    // start pierwszej rundy
    games->start_round();
    round = games->get_round();

    return true;
}

void GameState::end_game() {
    // kończymy aktywną grę i przerzucamy graczy z gry do lobby (żeby mogli grać znowu)
    if (games.has_value()) {
        // reset stanu graczy z gry i wrzucenie do lobby
        for (auto& p : games->players_list) {
            p.reset_state();
            players_list.push_back(p);
        }
        games.reset();
    }

    // reset liczb i czasu
    round = 0;
    round_end_time = 0;
    game_start_time = 0;
    num_players = static_cast<int>(players_list.size());
}

bool GameState::is_game_active() const {
    return games.has_value() && games->is_game_active();
}
