// plik game.cpp
#include "game.h"
#include <utility>
#include <ctime>

#include "server/cron/cron.h"

// Tworzymy grę na podstawie listy graczy (już w rozgrywce) i czasu rundy
Game::Game(std::vector<Player> player, time_t round_duration):
      round_duration(round_duration),
      players_list(std::move(player)){

    Cron& cron = Cron::instance();
    cron.set_job_interval("round_finish", std::chrono::seconds(round_duration));

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

    Cron& cron = Cron::instance();
    cron.reset_job_next_run("round_finish");
    cron.set_job_mode("round_finish", Cron::JobMode::ONCE);

    // Jeśli gra już powinna się skończyć, to nie startuj nowej rundy
    if (check_if_game_is_over()) {
        return false;
    }

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
    rounds.emplace_back(alive, round_duration);
    return true;
}

// Kończy aktualną rundę i w razie potrzeby startuje następną
bool Game::end_round() {

    // 1) ZAMKNIJ RUNDĘ: eliminuj wszystkich co nie zgadli do końca czasu
    if (!rounds.empty()) {
        rounds.back().finalize_round();
    }

    // 2) (opcjonalnie) statystyki / sumowanie błędów jak masz
    for (auto& p : players_list) {
        if (p.is_alive) {
            p.handle_round();
        }
    }

    // 3) Po eliminacjach sprawdź czy koniec gry
    if (check_if_game_is_over()) {
        return false;
    }

    // 4) Start kolejnej rundy
    return start_round();
}


// Gra się kończy gdy jest <= 1 żywy gracz
bool Game::check_if_game_is_over() {
    int alive_players = 0;
    for (const auto& p : players_list) {
        auto current_round = rounds.back();
        //current_round.
        if (p.is_alive ) alive_players++;
    }
    return alive_players <= 1;
}

int Game::get_round() const {
    return rounds.size();
}


Result<std::vector<WordleWord>> Game::make_guess(const std::string& player_name,
                                                 const std::string& guess,
                                                 std::time_t client_ts) {
    if (rounds.empty()) {
        if (!start_round())
            return Error("Failed to start round", HttpStatusCode::INTERNAL_SERVER_ERROR);
    }

    if (!rounds.back().is_round_active()) {
        const bool next_started = end_round();
        if (!next_started) {

            return Result<std::vector<WordleWord>>(std::vector<WordleWord>{});
        }
    }

    Player* p = find_player_ptr_by_name(player_name);
    if (!p) return Error("Player not found", HttpStatusCode::NOT_FOUND);
    if (!p->is_alive) return Error("Player eliminated", HttpStatusCode::FORBIDDEN);

    // !!! REFERENCJA, nie kopia
    Round& r = rounds.back();

    auto res = r.make_guess(p, guess, client_ts);
    if (res.is_err()) return res.unwrap_err();

    // kopiujemy historię zanim ewentualnie dojdzie nowa runda
    auto history = res.unwrap();

    // jeśli wszyscy żywi zgadli -> kończ rundę i startuj następną
    if (r.check_if_round_is_over()) {
        const bool next_started = end_round();
        (void)next_started;
    }

    return Result<std::vector<WordleWord>>(history);
}


