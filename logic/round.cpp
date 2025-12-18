#include "round.h"
#include <random>

// Losuje jedno słowo z listy (na razie “dummy” baza)
static std::string pick_random_word_5() {
    static const std::vector<std::string> words = {
        "apple", "grape", "lemon", "mango", "pearl",
        "bread", "chair", "zebra", "piano", "stone",
        "night", "light", "water", "candy", "snake"
    };

    static std::mt19937 rng(std::random_device{}());

    // To tworzy “generator liczb całkowitych” w zakresie [0, words.size()-1]
    // żeby wylosować indeks słowa z wektora.
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);

    return words[dist(rng)];
}

/*
    Konstruktor rundy:
    - dostaje listę graczy, którzy grają w tej rundzie (Player*).
    - losuje hasło (word)
    - ustawia czas końca rundy
    - tworzy dla każdego gracza obiekt Guesses (np. 6 prób)
*/
Round::Round(std::vector<Player*> player_list, int /*num_players*/, time_t round_duration)
    : word(pick_random_word_5()),
      round_duration(round_duration) {

    game_start_time = std::time(nullptr);
    round_end_time = game_start_time + round_duration;

    for (Player* p : player_list) {
        if (!p) continue;
        players_map.emplace(p, Guesses(6));
        p->round_errors = 0; // Round jest friend Player, więc może resetować
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
void Round::make_guess(Player* player, std::string& guess) {
    if (!player) return;
    if (!is_round_active()) return;
    if (!player->is_alive) return;

    auto it = players_map.find(player);
    if (it == players_map.end()) return;

    Guesses& g = it->second;

    if (g.is_lost()) return; // nie ma już prób

    GuessResult res = g.add_guess_word(guess, word);

    if (res == GuessResult::ADDED) {
        player->round_errors += 1; // błąd tylko gdy nie trafił
    }
    // CORRECT -> brak błędu
    // ERR -> ignorujesz (np. zła długość / brak prób)
}
