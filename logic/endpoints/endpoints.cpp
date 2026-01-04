#include "logic/endpoints/request_bodies.h"
#include "server/http/server_method.h"
#include <memory>
#include "logic/game_state.h"
#include "server/web-socket/web_socket_pool.h"
#include "server/cron/cron.h"
#include "server/utils/logger.h"

GameState game_state = GameState(60);

void set_game_state_cron() {
    Cron& cron = Cron::instance();
    cron.add_job(
        "round_finish", 
        []() {
            Logger::instance().info("Round finished");
            game_state.next_round();
            nlohmann::json json = game_state;
            WebSocketPool::instance().broadcast_all(json);
        },
   std::chrono::seconds(60), Cron::JobMode::OFF)
   .add_job(
    "vote_end",
    []() {
        game_state.end_vote();
        nlohmann::json json = game_state;
        WebSocketPool::instance().broadcast_all(json);
    },
    std::chrono::seconds(60), Cron::JobMode::OFF);
};

ServerMethod join_method = ServerMethod<JoinRequest>("/join", HttpMethod::POST, 
[](const JoinRequest& request) {
    //gracz wchodzi do gry wchodzi do poczekalni jesli jego nick jest juz zajety to zwraca error

    auto result = game_state.add_player(request);
    //game_state.start_game(); //tymczasowo dla testow

    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }
    nlohmann::json json = game_state;
    WebSocketPool::instance().broadcast_all(json);

    return Result<nlohmann::json>(json);
});

ServerMethod leave_method = ServerMethod<JoinRequest>("/leave", HttpMethod::DELETE, 
[](const JoinRequest& request) {
    auto result = game_state.remove_player(request);
    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }
    nlohmann::json json = game_state;
    WebSocketPool::instance().broadcast_all(json);

    return Result<nlohmann::json>(json);
});

ServerMethod ready_method = ServerMethod<StateRequest>("/ready", HttpMethod::POST,
[](const StateRequest& request) {
    // ustaw gracza jako READY w lobby
    auto result = game_state.set_ready(request);

    if (result.is_err()) {
        return Result<nlohmann::json>(result.unwrap_err());
    }

    nlohmann::json json = game_state;
    WebSocketPool::instance().broadcast_all(json);
    return Result<nlohmann::json>(json);
});


ServerMethod state_method = ServerMethod<StateRequest>("/", HttpMethod::GET, 
[](const StateRequest& request) {
    // pobiera stan gry dostepny dla gracza zwraca error jesli gracz nie jest w grze
    nlohmann::json json = game_state;
    // return Result<nlohmann::json>(json);

    return Result<nlohmann::json>(game_state);
});

ServerMethod guess_method = ServerMethod<GuessRequest>("/guess", HttpMethod::POST,
[](const GuessRequest& request) {
    auto result = game_state.make_guess(request);
    if (result.is_err()) return Result<nlohmann::json>(result.unwrap_err());

    nlohmann::json json;
    json["state"] = game_state;
    json["guess_result"] = result.unwrap();  //tablica WordleWord
    WebSocketPool::instance().broadcast_all(json);
    return Result<nlohmann::json>(json);
});

ServerMethod vote_method = ServerMethod<VoteRequest>("/vote", HttpMethod::POST,
[](const VoteRequest& request) {
    
    auto result = game_state.vote(
        request.voting_player,
         request.voted_player, 
         request.vote_for
        );
    if (result.is_err()) return Result<nlohmann::json>(result.unwrap_err());
    nlohmann::json json = game_state;
    WebSocketPool::instance().broadcast_all(json);
    return Result<nlohmann::json>(json);
});

