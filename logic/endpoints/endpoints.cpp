#include "logic/endpoints/request_bodies.h"
#include "server/server_method.h"
#include <memory>
#include "logic/game_state.h"
#include "server/web-socket/web_socket_pool.h"

GameState game_state = GameState(6,60);

ServerMethod join_method = ServerMethod<JoinRequest>("/join", HttpMethod::POST, 
[](const JoinRequest& request) {
    //gracz wchodzi do gry wchodzi do poczekalni jesli jego nick jest juz zajety to zwraca error

    auto result = game_state.add_player(request);
    game_state.start_game(); //tymczasowo dla testow

    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }
    nlohmann::json json = game_state;
    return Result<nlohmann::json>(json);
});

ServerMethod leave_method = ServerMethod<JoinRequest>("/join", HttpMethod::DELETE, 
[](const JoinRequest& request) {
    auto result = game_state.remove_player(request);
    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }
    nlohmann::json json = game_state;
    return Result<nlohmann::json>(json);
});

ServerMethod ready_method = ServerMethod<StateRequest>("/ready", HttpMethod::POST, 
    [](const StateRequest& request) {
        // gracze sa ready
        nlohmann::json json = game_state;
        return Result<nlohmann::json>(json);
    });


ServerMethod state_method = ServerMethod<StateRequest>("/", HttpMethod::GET, 
[](const StateRequest& request) {
    // pobiera stan gry dostepny dla gracza zwraca error jesli gracz nie jest w grze
    nlohmann::json json = game_state;
    // return Result<nlohmann::json>(json);

    WebSocketPool::instance().broadcast_all(json);
    return Result<nlohmann::json>(nlohmann::json());
});

ServerMethod guess_method = ServerMethod<GuessRequest>("/guess", HttpMethod::POST, 
[](const GuessRequest& request) { 
    //gracz zgaduje słowo zwraca nowy stan gry error jesli gracz nie jest w grze lub odpadl/w poczekalni
    auto result = game_state.make_guess(request);
    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }
    nlohmann::json json = game_state;
    return Result<nlohmann::json>(json);
});
