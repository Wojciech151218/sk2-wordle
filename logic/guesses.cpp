#include "guesses.h"

//konstruktor
Guesses::Guesses(int max_guesses) : max_guesses(max_guesses) {}

//sprawdza czy guess jest poprawny
bool Guesses::is_guess_correct(std::string guess, std::string actual) {
    return guess == actual;
}

//sprawdza czy gracz przegrał (zużył wszystkie próby)
bool Guesses::is_lost() {
    if (max_guesses <= 0) return true;
    return guesses.size() >= static_cast<size_t>(max_guesses);
}

//dodaje zgadywane słowo i koloruje litery
GuessResult Guesses::add_guess_word(std::string guess, std::string actual){
    std::vector<Letter> out;
    // walidacja zgadywanego słowa
    if (guess.length() != actual.length() || is_lost()) {
        return GuessResult::ERR;
    }

    // trafione - same zielone
    if (is_guess_correct(guess, actual)) {
        for (size_t i = 0; i < guess.length(); i++) {
            Letter letter;
            letter.letter = std::string(1, guess[i]);
            letter.type = LetterType::Green;
            out.push_back(letter);
        }
        guesses.push_back(out);
        return GuessResult::CORRECT;
    }

    // nietrafione - Green/Yellow/Gray 
    for (size_t i = 0; i < guess.length(); i++) {
        Letter letter;
        letter.letter = std::string(1, guess[i]);

        if (guess[i] == actual[i]) {
            letter.type = LetterType::Green;
        } else if (actual.find(guess[i]) != std::string::npos) {
            letter.type = LetterType::Yellow;
        } else {
            letter.type = LetterType::Gray;
        }
        out.push_back(letter);
    }

    guesses.push_back(out);
    return GuessResult::ADDED;  //znak, że próba dodana, ale błędna 
}
