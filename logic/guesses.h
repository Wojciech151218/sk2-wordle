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
    Result<WordleWord> add_guess_word(const std::string& guess, const std::string& actual);


    // czy gracz przegrał 
    bool is_lost();

    const std::vector<WordleWord>& get_history() const { return guesses; }
};
