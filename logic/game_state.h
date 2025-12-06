// game_state.h

#pragma once

#include <vector>
#include <string>
#include <ctime>  // for time_t type

class GameState {
private:
    int num_players; // Liczba graczy
    int round; // Numer rundy
    time_t round_end_time; // Czas do końca rundy
    time_t round_duration; // Czas trwania jednej rundy
    time_t game_start_time; // Czas rozpoczęcia gry
    bool is_game_running; // Flaga mówiąca, czy gra jest w trakcie

    struct Player { // Struktura przechowująca dane gracza
        std::string player_name; // Nazwa gracza
        int round_errors; // Liczba błędów w rundzie
        int all_errors; // Łączna liczba błędów
        bool is_active; // Aktywność gracza (czy bierze udział w grze)
        bool is_alive; // Czy gracz żyje
    };

    std::vector<Player> players_list; // Lista graczy

public:
    GameState(int num_players, time_t round_duration);
    
    bool add_player(const std::string& player_name); // Dodawanie gracza
    bool start_game(); // Rozpoczęcie gry
    bool end_round(); // Zakończenie rundy
    bool check_if_game_is_over(); // Sprawdzenie, czy gra się zakończyła
    void end_game(); // Zakończenie gry
    bool remove_player(const std::string& player_name); // Usunięcie gracza

    int get_round() const; // Pobranie numeru rundy
    bool is_game_active() const; // Sprawdzenie, czy gra trwa
};
