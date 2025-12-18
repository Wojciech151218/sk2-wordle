#include "wordle_word.h"

#include <cstdlib>

WordleWord WordleWord::get_random_colors(const std::string& word) {
    WordleWord wordle_word;
    static const std::vector<LetterType> types = {
        LetterType::Yellow,
        LetterType::Green,
        LetterType::Gray
    };

    for (char letter : word) {
        wordle_word.letters.push_back({
            std::string(1, letter),
            types[rand() % types.size()]
        });
    }

    return wordle_word;
}

WordleWord WordleWord::from_guess(const std::string& guess, const std::string& actual) {
    WordleWord wordle_word;

    if (guess == actual) {
        for (size_t i = 0; i < guess.length(); i++) {
            Letter letter;
            letter.letter = std::string(1, guess[i]);
            letter.type = LetterType::Green;
            wordle_word.letters.push_back(letter);
        }
        return wordle_word;
    }

    for (size_t i = 0; i < actual.length(); i++) {
        Letter letter;
        letter.letter = std::string(1, guess[i]);
        if (guess[i] == actual[i]) {
            letter.type = LetterType::Green;
        } else if (actual.find(guess[i]) != std::string::npos) {
            letter.type = LetterType::Yellow;
        } else {
            letter.type = LetterType::Gray;
        }
        wordle_word.letters.push_back(letter);
    }

    return wordle_word;
}


bool WordleWord::is_green() const {
    for (const auto& letter : letters) {
        if (letter.type != LetterType::Green) return false;
    }
    return true;
}