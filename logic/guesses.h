// game_state.h

#pragma once

#include <vector>
#include <string>
#include <ctime>  

enum class LetterType {
    Yellow,
    Green,
    Gray,
};

struct Letter {
    std::string letter;
    LetterType type;
};


class Guesses {
private:
    std::vector<std::vector<Letter>> guesses;   // Lista zgadywanych słów
    int max_guesses;                            // Maksymalna liczba zgadywanych słów

public:
    Guesses(int max_guesses);// : max_guesses(max_guesses) {};
    
    void add_guess_word(std::string guess, std::string actual); // Dodaje zgadywane słowo

    bool is_guess_correct(); // Sprawdza, czy zgadywane słowo jest poprawne
    bool is_lost();          // Sprawdza, czy gracz przegrał

    
};
