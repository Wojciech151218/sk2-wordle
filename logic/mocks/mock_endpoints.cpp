#include "logic/mocks/mock_endpoints.h"
#include "logic/endpoints/request_bodies.h"
#include "server/server/server_method.h"
#include "server/utils/logger.h"
#include "server/web-socket/web_socket_pool.h"
#include "server/cron/cron.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

namespace {

constexpr std::time_t MOCK_ROUND_DURATION = 60;

nlohmann::json make_player(const std::string& name,
                           bool is_ready = true,
                           int round_errors = 0,
                           int all_errors = 0,
                           bool is_alive = true) {
    return {
        {"player_name", name},
        {"round_errors", round_errors},
        {"all_errors", all_errors},
        {"is_alive", is_alive},
        {"is_ready", is_ready}
    };
}

std::vector<nlohmann::json> base_lobby_players() {
    return {
        make_player("MockPlayer1", true, 0, 0, true),
        make_player("MockPlayer2", true, 1, 1, true)
    };
}

nlohmann::json make_round(const std::string& word,
                          std::time_t round_start_time,
                          std::time_t duration) {
    return {
        {"word", word},
        {"round_end_time", round_start_time + duration},
        {"round_duration", duration},
        {"round_start_time", round_start_time}
    };
}

nlohmann::json make_game(const std::vector<nlohmann::json>& players_in_game,
                         const nlohmann::json& round) {
    return {
        {"round_end_time", round["round_end_time"]},
        {"round_duration", round["round_duration"]},
        {"game_start_time", round["round_start_time"]},
        {"players_list", players_in_game},
        {"rounds", nlohmann::json::array({round})}
    };
}

nlohmann::json create_mock_lobby_state(const std::vector<nlohmann::json>& lobby_players) {
    return {
        {"round_end_time", 0},
        {"round_duration", MOCK_ROUND_DURATION},
        {"game_start_time", 0},
        {"players_list", lobby_players},
        {"game", nullptr}
    };
}

nlohmann::json create_mock_active_state(const std::vector<nlohmann::json>& game_players) {
    const auto round_start = std::time(nullptr) - 15;
    const auto round = make_round("MOCK", round_start, MOCK_ROUND_DURATION);
    const auto game_json = make_game(game_players, round);

    return {
        {"round_end_time", game_json["round_end_time"]},
        {"round_duration", game_json["round_duration"]},
        {"game_start_time", game_json["game_start_time"]},
        {"players_list", nlohmann::json::array()}, // lobby emptied after start
        {"game", game_json}
    };
}

nlohmann::json create_mock_guess_history() {
    return nlohmann::json::array({
        {
            {"letters", nlohmann::json::array({
                {{"letter", "M"}, {"type", "green"}},
                {{"letter", "O"}, {"type", "yellow"}},
                {{"letter", "C"}, {"type", "gray"}},
                {{"letter", "K"}, {"type", "green"}},
                {{"letter", "S"}, {"type", "gray"}}
            })}
        },
        {
            {"letters", nlohmann::json::array({
                {{"letter", "G"}, {"type", "yellow"}},
                {{"letter", "A"}, {"type", "gray"}},
                {{"letter", "M"}, {"type", "green"}},
                {{"letter", "E"}, {"type", "green"}},
                {{"letter", "S"}, {"type", "gray"}}
            })}
        }
    });
}

}  // namespace

std::unique_ptr<Cron> get_mock_game_cron() {
    std::unique_ptr<Cron> cron = std::make_unique<Cron>();
    cron->add_job([]() {
        nlohmann::json json = create_mock_active_state({
            make_player("MockPlayer1", true, 0, 0, true),
            make_player("MockPlayer2", true, 1, 1, true)
        });
        Logger::instance().debug("Mock: Sending game state to all clients : " + json.dump());
        WebSocketPool::instance().broadcast_all(json);
    }, std::chrono::seconds(60));
    return cron;
}

ServerMethod<JoinRequest> mock_join_method = ServerMethod<JoinRequest>("/join", HttpMethod::POST, 
[](const JoinRequest& request) {
    Logger::instance().debug("Mock join endpoint called for player: " + request.player_name);
    
    auto players = base_lobby_players();
    players.push_back(make_player(request.player_name, false, 0, 0, true));

    return Result<nlohmann::json>(create_mock_lobby_state(players));
});

ServerMethod<JoinRequest> mock_leave_method = ServerMethod<JoinRequest>("/leave", HttpMethod::DELETE, 
[](const JoinRequest& request) {
    Logger::instance().debug("Mock leave endpoint called for player: " + request.player_name);
    
    auto players = base_lobby_players();
    players.erase(
        std::remove_if(
            players.begin(),
            players.end(),
            [&](const nlohmann::json& player) { return player["player_name"] == request.player_name; }
        ),
        players.end()
    );

    return Result<nlohmann::json>(create_mock_lobby_state(players));
});

ServerMethod<StateRequest> mock_ready_method = ServerMethod<StateRequest>("/ready", HttpMethod::POST, 
[](const StateRequest& request) {
    Logger::instance().debug("Mock ready endpoint called for player: " + request.player_name);
    
    auto players = base_lobby_players();
    players.push_back(make_player(request.player_name, true, 0, 0, true));

    return Result<nlohmann::json>(create_mock_lobby_state(players));
});

ServerMethod<StateRequest> mock_state_method = ServerMethod<StateRequest>("/", HttpMethod::GET, 
[](const StateRequest& request) {
    Logger::instance().debug("Mock state endpoint called for player: " + request.player_name);
    
    nlohmann::json json = create_mock_active_state({
        make_player("MockPlayer1", true, 0, 0, true),
        make_player("MockPlayer2", true, 1, 1, true),
        make_player(request.player_name, true, 0, 0, true)
    });
    return Result<nlohmann::json>(json);
});

ServerMethod<GuessRequest> mock_guess_method = ServerMethod<GuessRequest>("/guess", HttpMethod::POST, 
[](const GuessRequest& request) {
    Logger::instance().debug("Mock guess endpoint called for player: " + request.player_name + " with guess: " + request.guess);
    
    nlohmann::json response;
    response["state"] = create_mock_active_state({
        make_player("MockPlayer1", true, 0, 0, true),
        make_player("MockPlayer2", true, 1, 1, true),
        make_player(request.player_name, true, 0, 0, true)
    });
    response["guess_result"] = create_mock_guess_history();

    return Result<nlohmann::json>(response);
});

