#pragma once
#include <string>

class Game;
class Round;
class GameState;

/*
    Player trzyma stan konkretnego gracza
    round_errors rośnie w Round::make_guess() gdy GuessResult == ADDED
*/
class Player {
private:
    std::string player_name;
    int round_errors;   // ile błędów w tej rundzie 
    int all_errors;     // suma błędów z całej gry
    bool is_alive;      // czy gracz jest nadal w grze 

    // Dzięki temu Game/Round/GameState mogą edytować prywatne pola gracza
    friend class Game;
    friend class Round;
    friend class GameState;

public:
    Player(const std::string& name);

    // Reset gracza do stanu początkowego
    void reset_state();

    // Czy żyje
    bool get_is_alive();

    // Podsumowanie rundy:
    void handle_round();

    void set_is_alive(bool status);
    void set_round_errors(int errors);
};
