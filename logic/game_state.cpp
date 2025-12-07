// game_state.cpp

#include "game_state.h"
#include <iostream>
#include <ctime>

// Konstruktor, inicjalizuje wartości początkowe
GameState::GameState(int num_players, time_t round_duration)
    : num_players(num_players), round(0), round_duration(round_duration), 
      is_game_running(false) {}

// Dodaje gracza do listy, jeśli gra jeszcze nie trwa
bool GameState::add_player(const std::string& player_name) {
    if (!is_game_running) {  // Sprawdzamy, czy gra nie jest w toku
        Player new_player = {player_name, 0, true, true};  // Tworzymy nowego gracza
        num_players++; // Zwiększamy liczbę graczy
        players_list.push_back(new_player);  // Dodajemy gracza do listy
        return true;
    }
    return false;
}

// Rozpoczyna grę, jeśli jest odpowiednia liczba graczy
bool GameState::start_game() {
    if (!is_game_running && num_players >= 3) {  // Gra nie jest w toku i mamy odpowiednią liczbę graczy
        is_game_running = true;  // Rozpoczynamy grę
        game_start_time = std::time(nullptr);  // Ustawiamy czas rozpoczęcia gry
        round_end_time = game_start_time + round_duration;  // Ustawiamy czas do końca rundy

        // Rozpoczynamy pierwszą rundę
        bool round_started = end_round(); // uruchamiamy metodę do zakończenia pierwszej rundy

        if (!round_started) {
            return false;  // Jeśli nie udało się zakończyć rundy, nie rozpoczynamy gry
        }

        return true; // Gra rozpoczęta
    }
    return false; // Gra już trwa lub za mało graczy
}


// Zakończenie rundy, przechodzi do następnej
bool GameState::end_round() {
    if (is_game_running) {  // Gra trwa
        round++;  // Zwiększamy numer rundy
        round_end_time = std::time(nullptr) + round_duration;  // Ustawiamy nowy czas końca rundy

        // Sprawdzamy status graczy
        for (auto& player : players_list) {
            player.all_errors += player.round_errors;  // Dodajemy błędy z tej rundy do całkowitych błędów
            if (player.is_active && player.round_errors >= 6) {  // Jeśli gracz ma 6 błędów, zostaje wyeliminowany
                player.is_alive = false;
            }
            player.round_errors = 0;  // Resetujemy liczbę błędów w tej rundzie
        }

        // Sprawdzamy, czy gra się zakończyła
        if (check_if_game_is_over()) {
            return false;  // Jeśli gra się zakończyła, nie przechodzimy do kolejnej rundy
        }

        return true;  // Runda zakończona, przechodzimy do następnej
    }
    return false;  // Gra nie trwa
}

// Sprawdzamy, czy gra się zakończyła (czy tylko jeden gracz żyje)
bool GameState::check_if_game_is_over() {
    int alive_players = 0;
    for (const auto& player : players_list) {
        if (player.is_alive) {
            alive_players++;  // Zliczamy żywych graczy
        }
    }
    if (alive_players <= 1) {  // Gra kończy się, jeśli tylko jeden gracz jest żywy
        is_game_running = false;  // Zatrzymujemy grę
        return true;
    }
    return false;
}

// Zakończenie gry, resetowanie stanu
void GameState::end_game() {
    is_game_running = false;  // Zatrzymujemy grę
    round = 0;  // Resetujemy numer rundy
    for (auto& player : players_list) {
        player.all_errors = 0;  // Resetujemy liczbę błędów
        player.is_active = true;  // Ustawiamy gracza jako aktywnego
        player.is_alive = true;  // Ustawiamy gracza jako żywego
    }
}

// Zwracamy numer rundy
int GameState::get_round() const {
    return round;
}

// Zwracamy, czy gra trwa
bool GameState::is_game_active() const {
    return is_game_running;
}
