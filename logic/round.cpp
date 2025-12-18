#include "round.h"
#include <random>

static std::string pick_random_word_5() {
    static const std::vector<std::string> words = {
        "apple", "grape", "lemon", "mango", "pearl",
        "bread", "chair", "zebra", "piano", "stone",
        "night", "light", "water", "candy", "snake"
    };

    static std::mt19937 rng(std::random_device{}());

    // generator liczb całkowitych w zakresie [0, words.size()-1]
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);

    return words[dist(rng)];
}

/*
    Konstruktor rundy
*/
Round::Round(std::vector<Player*> player_list, time_t round_duration)
    : word(pick_random_word_5()),
      round_duration(round_duration) {

    round_start_time = std::time(nullptr);
    round_end_time = round_start_time + round_duration;

    for (Player* p : player_list) {
        if (!p) continue;
        players_map.emplace(p, Guesses(6));
    }
}

bool Round::is_round_active() {
    return std::time(nullptr) < round_end_time;
}

/*
    make_guess:
    - sprawdza czy runda trwa, gracz istnieje i żyje
    - znajduje “guessy” tego gracza w mapie
    - dodaje próbę do Guesses
    - jeśli próba była błędna (ADDED), zwiększa round_errors gracza
*/
Result<Round> Round::make_guess(Player* player, std::string& guess) {
    if (!player) return Error("Player not in round", HttpStatusCode::NOT_FOUND);                //gracz istnieje
    if (!is_round_active()) return Error("Round not active", HttpStatusCode::BAD_REQUEST);     //czy runda trwa,
    if (!player->is_alive) return Error("Player not alive", HttpStatusCode::BAD_REQUEST);      //gracz żyje

    auto it = players_map.find(player);
    if (it == players_map.end()) return Error("Player not in round", HttpStatusCode::NOT_FOUND); //jesli gracz nie bierze udziału w rundzie

    Guesses& g = it->second;

    if (g.is_lost()) return Error("Player lost", HttpStatusCode::BAD_REQUEST); // nie ma już prób

    auto result = g.add_guess_word(guess, word);
    if (result.is_err()) return Error(result.unwrap_err());

    if (result.unwrap() == GuessResult::INCORRECT) {// jeśli próba była błędna (ADDED), zwiększa round_errors gracza
        player->round_errors += 1; 
    }
    // CORRECT - brak user odgadł hasło ---- trzeba dopisać logike na to jak odgadł hasło ale to już po stronie klienta chyba
    // ERR -  zła długość / brak prób
}
