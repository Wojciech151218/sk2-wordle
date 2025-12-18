#include "guesses.h"

//konstruktor
Guesses::Guesses(int max_guesses) : max_guesses(max_guesses) {}


//sprawdza czy gracz przegrał (zużył wszystkie próby)
bool Guesses::is_lost() {
    return guesses.size() >= static_cast<size_t>(max_guesses);
}

//dodaje zgadywane słowo i koloruje litery
Result<GuessResult> Guesses::add_guess_word(std::string guess, std::string actual){
    WordleWord out = WordleWord::from_guess(guess, actual);
    // walidacja zgadywanego słowa
    if (guess.length() != actual.length()) {
        return Error("guess has wrong length", HttpStatusCode::BAD_REQUEST);
    }
    if (is_lost()) {
        return Error("player lost", HttpStatusCode::BAD_REQUEST);
    }

    guesses.push_back(out);

    if (out.is_green()) {
        return Result<GuessResult>(GuessResult::CORRECT);
    }
    return Result<GuessResult>(GuessResult::INCORRECT);
}
