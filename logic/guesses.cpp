#include "guesses.h"

//konstruktor
Guesses::Guesses(int max_guesses) : max_guesses(max_guesses) {}


//sprawdza czy gracz przegrał (zużył wszystkie próby)
bool Guesses::is_lost() {
    return guesses.size() >= static_cast<size_t>(max_guesses);
}

//dodaje zgadywane słowo i koloruje litery
Result<WordleWord> Guesses::add_guess_word(const std::string& guess, const std::string& actual) {
    if (guess.length() != actual.length())
        return Error("guess has wrong length", HttpStatusCode::BAD_REQUEST);

    if (is_lost())
        return Error("player lost", HttpStatusCode::BAD_REQUEST);

    WordleWord out = WordleWord::from_guess(guess, actual);
    guesses.push_back(out);
    return Result<WordleWord>(out);
}
