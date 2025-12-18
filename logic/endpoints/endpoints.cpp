#include "logic/endpoints/request_bodies.h"
#include "server/server_method.h"
#include <memory>

//WordleState wordle_state = WordleState(6,5);

ServerMethod join_method = ServerMethod<JoinRequest>("/join", HttpMethod::POST, 
[](const JoinRequest& request) {
    //gracz wchodzi do gry wchodzi do poczekalni jesli jego nick jest juz zajety to zwraca error
    

    return Result<nlohmann::json>(nlohmann::json({
        {"status", "success"},
        {"player_name", request.player_name}
    }));
});

ServerMethod state_method = ServerMethod<StateRequest>("/", HttpMethod::GET, 
[](const StateRequest& request) {
    // pobiera stan gry dostepny dla gracza zwraca error jesli gracz nie jest w grze

    return Result<nlohmann::json>(nlohmann::json({
        {"status", "success"},
        {"player_name", request.player_name},
        {"timestamp", request.timestamp}
    }));
});

ServerMethod guess_method = ServerMethod<GuessRequest>("/guess", HttpMethod::POST, 
[](const GuessRequest& request) {
    //gracz zgaduje słowo zwraca nowy stan gry error jesli gracz nie jest w grze lub odpadl/w poczekalni
    
    return Result<nlohmann::json>(nlohmann::json({
        {"status", "success"},
        {"player_name", request.player_name},
        {"timestamp", request.timestamp},
        {"guess", request.guess}
    }));
});
