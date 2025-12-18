#include "logic/word_request.h"
#include "server/server_method.h"
#include "logic/wordle_types.h"
#include <memory>

WordleState wordle_state = WordleState(6,5);

ServerMethod join_method = ServerMethod<WordRequest>("/join", HttpMethod::POST, 
[](const WordRequest& request) {
    //gracz wchodzi do gry wchodzi do poczekalni jesli jego nick jest juz zajety to zwraca error
    wordle_state.add_word(WordleWord::get_random_colors(request.word));

    //return Error("heszke w meszke", HttpStatusCode::INTERNAL_SERVER_ERROR);
    return Result<nlohmann::json>(nlohmann::json(wordle_state));
});

ServerMethod state_method = ServerMethod<WordRequest>("/", HttpMethod::GET, 
[](const WordRequest& request) {
    // pobiera stan gry dostepny dla gracza zwraca error jesli gracz nie jest w grze
    wordle_state.add_word(WordleWord::get_random_colors(request.word));

    //return Error("heszke w meszke", HttpStatusCode::INTERNAL_SERVER_ERROR);
    return Result<nlohmann::json>(nlohmann::json(wordle_state));
});

ServerMethod guess_method = ServerMethod<WordRequest>("/guess", HttpMethod::POST, 
[](const WordRequest& request) {
    //gracz zgaduje słowo zwraca nowy stan gry error jesli gracz nie jest w grze lub odpadl/w poczekalni
    wordle_state.add_word(WordleWord::get_random_colors(request.word));

    //return Error("heszke w meszke", HttpStatusCode::INTERNAL_SERVER_ERROR);
    return Result<nlohmann::json>(nlohmann::json(wordle_state));
});
