// plik game.cpp
#include "game.h"
#include <utility>
#include <ctime>

// Tworzymy grę na podstawie listy graczy (już w rozgrywce) i czasu rundy
Game::Game(std::vector<Player> player, time_t round_duration)
    : round(0),
      round_duration(round_duration),
      players_list(std::move(player)),
      is_game_running(true) {

    // ustawienia czasu rund i gry
    game_start_time = std::time(nullptr);

    round_end_time = game_start_time + round_duration;
}

// Znajduje gracza po nazwie i zwraca wskaźnik na obiekt w players_list
Player* Game::find_player_ptr_by_name(const std::string& name) {
    for (auto& p : players_list) {
        if (p.player_name == name) return &p; // friend (dostęp do private)
    }
    return nullptr;
}

// Startuje nową rundę
bool Game::start_round() {
    if (!is_game_running) return false;

    // Jeśli gra już powinna się skończyć, to nie startuj nowej rundy
    if (check_if_game_is_over()) {
        is_game_running = false;
        return false;
    }

    round++; // kolejna runda
    round_end_time = std::time(nullptr) + round_duration;

    // wskaźniki na żywych graczy, bo Round trzyma Player* w mapie
    std::vector<Player*> alive;
    alive.reserve(players_list.size());

    for (auto& p : players_list) {
        if (p.is_alive) {
            p.round_errors = 0;    // reset błędów na nową rundę
            alive.push_back(&p);   // zapisujemy adres obiektu w wektorze
        }
    }

    // Tworzymy rundę i dopisujemy do listy rund
    rounds.emplace_back(alive, static_cast<int>(alive.size()), round_duration);
    return true;
}

// Kończy aktualną rundę i w razie potrzeby startuje następną
bool Game::end_round() {
    if (!is_game_running) return false;

    // Każdy gracz podsumowuje rundę
    for (auto& p : players_list) {
        if (p.is_alive) {
            p.handle_round();
        }
    }

    // Jeśli po podsumowaniu zostało <= 1 gracz, kończymy grę
    if (check_if_game_is_over()) {
        is_game_running = false;
        return false;
    }

    // W przeciwnym razie automatycznie startujemy kolejną rundę
    return start_round();
}

// Gra się kończy gdy jest <= 1 żywy gracz
bool Game::check_if_game_is_over() {
    int alive_players = 0;
    for (const auto& p : players_list) {
        if (p.is_alive) alive_players++;
    }
    return alive_players <= 1;
}

int Game::get_round() const {
    return round;
}

bool Game::is_game_active() const {
    return is_game_running;
}

// Obsługa guess:  /////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!   NIE WIEM CZY DOBRZE  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///////////////////
void Game::make_guess(std::string player_name, std::string guess) {
    if (!is_game_running) return;

    // Jeśli nie ma aktywnej rundy, to uruchamiamy pierwszą
    if (rounds.empty()) {
        if (!start_round()) return;
    }

    // Jeśli czas rundy minął, kończymy rundę (co odpali kolejną)
    if (!rounds.back().is_round_active()) {
        end_round();
        if (!is_game_running) return;
        if (rounds.empty()) return;
    }

    // Szukamy gracza po nicku
    Player* p = find_player_ptr_by_name(player_name);
    if (!p || !p->is_alive) return;

    // Przekazujemy do aktualnej rundy: Round doda guess i ewentualnie zwiększy round_errors
    rounds.back().make_guess(p, guess);
}
