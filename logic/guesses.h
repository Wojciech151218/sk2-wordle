#pragma once
#include <vector>
#include <string>

/*
    GuessResult:
    - ERR      - nie przyjęto próby (np. zła długość słowa lub brak prób)
    - CORRECT  - zgadnięte hasło
    - ADDED    - próba dodana, ale niepoprawna (czyli błąd gracza rośnie)
*/
enum class GuessResult {
    ERR,
    CORRECT,
    ADDED
};

enum class LetterType {
    Yellow,
    Green,
    Gray,
};


//Letter opisuje jedną literę z jej kolorem:
struct Letter {
    std::string letter;
    LetterType type;
};


class Guesses {
private:
    std::vector<std::vector<Letter>> guesses;
    int max_guesses;

public:
    Guesses(int max_guesses);

    // Dodaje próbę i koloruje litery na podstawie actual
    GuessResult add_guess_word(std::string guess, std::string actual);

    // walidacja zgadywanego słowa
    bool is_guess_correct(std::string guess, std::string actual);

    // czy gracz przegrał 
    bool is_lost();
};
