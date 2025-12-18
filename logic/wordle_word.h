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

    static WordleWord get_random_colors(const std::string& word);
    static WordleWord from_guess(const std::string& guess, const std::string& actual);
    bool is_green() const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WordleWord, letters);
};
