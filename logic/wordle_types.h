#pragma once

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

enum class LetterType {
    Yellow,
    Green,
    Gray,
};

NLOHMANN_JSON_SERIALIZE_ENUM(LetterType, {
    {LetterType::Yellow, "yellow"},
    {LetterType::Green,  "green"},
    {LetterType::Gray,   "gray"},
});

struct Letter {
    std::string letter;
    LetterType type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Letter, letter, type);
};

struct WordleWord {
    std::vector<Letter> letters;

    static WordleWord get_random_colors(const std::string& word) {
        WordleWord wordle_word;
        for (char letter : word) {
            // Assign a random LetterType (Yellow, Green, or Gray) to each letter
            static const std::vector<LetterType> types = {LetterType::Yellow, LetterType::Green, LetterType::Gray};
            wordle_word.letters.push_back({
                std::string(1, letter),
                types[rand() % types.size()]
            });
        }
        return wordle_word;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WordleWord, letters);
}; 

class WordleState {
private:
    std::vector<WordleWord> words;
    size_t max_words;
    size_t letters_per_word;

public:
    WordleState(size_t max_words, size_t letters_per_word) : max_words(max_words), letters_per_word(letters_per_word) {}

    void add_word(const WordleWord& word) {
        if (words.size() < max_words)
            words.push_back(word);
        else {
            words.erase(words.begin());
            words.push_back(word);
        }
    }

    std::vector<WordleWord> get_words() const {
        return words;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WordleState, words, max_words,letters_per_word);
};
