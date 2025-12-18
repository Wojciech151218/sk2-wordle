#pragma once

#include <vector>
#include <string>

#include "server/utils/result.h"
#include "wordle_word.h"

enum class GuessResult {
    INCORRECT,
    CORRECT,
};

class Guesses {
private:
    std::vector<WordleWord> guesses;
    int max_guesses;

public:
    Guesses(int max_guesses);

    // Dodaje próbę i koloruje litery na podstawie actual
    Result<GuessResult> add_guess_word(std::string guess, std::string actual);

    // czy gracz przegrał 
    bool is_lost();
};
