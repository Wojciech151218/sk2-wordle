#include "round.h"
#include <random>

static std::string pick_random_word() {
    // Picks a random word of length 4, 5, or 6 (chosen randomly each round).
    static const std::vector<std::string> words4 = {
        "book", "tree", "lamp", "rain", "wind",
        "gold", "ship", "road", "fish", "game",
        "snow", "frog", "wolf", "milk", "ring",
        "sand", "card", "king", "mint", "bell"
    };

    static const std::vector<std::string> words5 = {
        "apple", "grape", "lemon", "mango", "pearl",
        "bread", "chair", "zebra", "piano", "stone",
        "night", "light", "water", "candy", "snake",
    };

    static std::mt19937 rng(std::random_device{}());

    static const std::vector<std::string> words6 = {
        "planet", "silver", "banana", "castle", "forest",
        "button", "dragon", "rabbit", "bright", "winter",
        "singer", "yellow", "magnet", "secret", "rocket"
    };

    // Randomly pick length: 4, 5, or 6
    std::uniform_int_distribution<int> len_dist(4, 6);
    const int len = len_dist(rng);

    const std::vector<std::string>& pool =
        (len == 4) ? words4 :
        (len == 5) ? words5 :
                    words6;

    // generator liczb całkowitych w zakresie [0, pool.size()-1]
    std::uniform_int_distribution<size_t> dist(0, pool.size() - 1);
    return pool[dist(rng)];
}



static std::string test_word = "test";

/*
    Konstruktor rundy
*/
Round::Round(std::vector<Player*> player_list, time_t round_duration)
    : word(pick_random_word()),
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
bool Round::check_if_round_is_over() const {
    for (auto& [player, guesses] : players_map) {
        if (!player) continue;
        if (!player->is_alive) continue; // już odpadł np. przez 6 błędów

        if(!guesses.has_won()) {
            return false;
        }
    }
    return true;
}
/*
    make_guess:
    - sprawdza czy runda trwa, gracz istnieje i żyje
    - znajduje “guessy” tego gracza w mapie
    - dodaje próbę do Guesses
    - jeśli próba była błędna (ADDED), zwiększa round_errors gracza
*/
Result<std::vector<WordleWord>> Round::make_guess(Player* player,
                                                  const std::string& guess,
                                                  std::time_t client_ts) {
    if (!player) return Error("Player not in round", HttpStatusCode::NOT_FOUND);

    // WALIDACJA po timestampie requestu:
    // guess jest ważny tylko jeśli client_ts należy do [start, end)
    if (client_ts < round_start_time || client_ts >= round_end_time) {
        return Error("Guess timestamp not in current round time window",
                     HttpStatusCode::BAD_REQUEST);
    }

    // (opcjonalnie) dalej możesz też bronić się czasem serwera:
    // jeśli runda już minęła serwerowo -> odrzuć
    if (!is_round_active()) {
        return Error("Round not active", HttpStatusCode::BAD_REQUEST);
    }

    if (!player->is_alive) return Error("Player not alive", HttpStatusCode::BAD_REQUEST);

    auto it = players_map.find(player);
    if (it == players_map.end())
        return Error("Player not in round", HttpStatusCode::NOT_FOUND);

    Guesses& g = it->second;

    // 403 jeśli gracz już zgadł w tej rundzie
    if (g.has_won()) {
        return Error("Player already solved the word in this round", HttpStatusCode::FORBIDDEN);
    }


    auto colored_res = g.add_guess_word(guess, word);
    if (colored_res.is_err())
        return colored_res.unwrap_err();

    WordleWord colored = colored_res.unwrap();

    if (!colored.is_green()) {
        player->round_errors += 1;
        if (player->round_errors >= 6) {
            player->is_alive = false;
        }
    }

    return Result<std::vector<WordleWord>>(g.get_history());
}

    // CORRECT - brak user odgadł hasło ---- trzeba dopisać logike na to jak odgadł hasło ale to już po stronie klienta chyba
    // ERR -  zła długość / brak prób



void Round::finalize_round() {
    // Koniec rundy: przeżywa tylko ten, kto zgadł (ma przynajmniej jeden all-green)
    for (auto& [player, guesses] : players_map) {
        if (!player) continue;
        if (!player->is_alive) continue; // już odpadł np. przez 6 błędów

        if (!guesses.has_won()) {
            player->is_alive = false;
        }
    }
}