#include "logic/endpoints/mock_endpoints.h"
#include "logic/endpoints/request_bodies.h"
#include "server/server/server_method.h"
#include "server/utils/logger.h"
#include "server/web-socket/web_socket_pool.h"
#include "server/cron/cron.h"
#include <ctime>
#include <chrono>
#include <memory>

// Helper function to create a mock game state JSON
nlohmann::json create_mock_game_state() {
    nlohmann::json json;
    
    // Mock lobby state
    json["max_players"] = 6;
    json["round_end_time"] = std::time(nullptr) + 60; // 60 seconds from now
    json["round_duration"] = 60;
    json["game_start_time"] = std::time(nullptr);
    
    // Mock players list
    json["players_list"] = nlohmann::json::array();
    json["players_list"].push_back({
        {"player_name", "MockPlayer1"},
        {"round_errors", 0},
        {"all_errors", 0},
        {"is_alive", true}
    });
    json["players_list"].push_back({
        {"player_name", "MockPlayer2"},
        {"round_errors", 1},
        {"all_errors", 1},
        {"is_alive", true}
    });
    
    // Mock game (optional, can be null)
    json["game"] = nullptr;
    
    return json;
}

// Helper function to create a mock game state with an active game
nlohmann::json create_mock_game_state_with_game() {
    nlohmann::json json = create_mock_game_state();
    
    // Mock active game
    nlohmann::json game_json;
    game_json["round_end_time"] = std::time(nullptr) + 45;
    game_json["round_duration"] = 60;
    game_json["game_start_time"] = std::time(nullptr) - 15;
    
    // Mock players in game
    game_json["players_list"] = nlohmann::json::array();
    game_json["players_list"].push_back({
        {"player_name", "MockPlayer1"},
        {"round_errors", 0},
        {"all_errors", 0},
        {"is_alive", true}
    });
    game_json["players_list"].push_back({
        {"player_name", "MockPlayer2"},
        {"round_errors", 1},
        {"all_errors", 1},
        {"is_alive", true}
    });
    
    // Mock rounds
    game_json["rounds"] = nlohmann::json::array();
    nlohmann::json round_json;
    round_json["word"] = "MOCK";
    round_json["round_end_time"] = std::time(nullptr) + 45;
    round_json["round_duration"] = 60;
    round_json["round_start_time"] = std::time(nullptr) - 15;
    game_json["rounds"].push_back(round_json);
    
    json["game"] = game_json;
    
    return json;
}

std::unique_ptr<Cron> get_mock_game_cron() {
    std::unique_ptr<Cron> cron = std::make_unique<Cron>();
    cron->add_job([]() {
        nlohmann::json json = create_mock_game_state_with_game();
        Logger::instance().debug("Mock: Sending game state to all clients : " + json.dump());
        WebSocketPool::instance().broadcast_all(json);
    }, std::chrono::seconds(60));
    return cron;
}

ServerMethod<JoinRequest> mock_join_method = ServerMethod<JoinRequest>("/join", HttpMethod::POST, 
[](const JoinRequest& request) {
    Logger::instance().debug("Mock join endpoint called for player: " + request.player_name);
    
    nlohmann::json json = create_mock_game_state();
    
    // Add the joining player to the mock players list
    json["players_list"].push_back({
        {"player_name", request.player_name},
        {"round_errors", 0},
        {"all_errors", 0},
        {"is_alive", true}
    });
    
    return Result<nlohmann::json>(json);
});

ServerMethod<JoinRequest> mock_leave_method = ServerMethod<JoinRequest>("/join", HttpMethod::DELETE, 
[](const JoinRequest& request) {
    Logger::instance().debug("Mock leave endpoint called for player: " + request.player_name);
    
    nlohmann::json json = create_mock_game_state();
    
    // Remove the player from the mock players list (simplified - just return state without the player)
    json["players_list"] = nlohmann::json::array();
    json["players_list"].push_back({
        {"player_name", "MockPlayer1"},
        {"round_errors", 0},
        {"all_errors", 0},
        {"is_alive", true}
    });
    
    return Result<nlohmann::json>(json);
});

ServerMethod<StateRequest> mock_ready_method = ServerMethod<StateRequest>("/ready", HttpMethod::POST, 
[](const StateRequest& request) {
    Logger::instance().debug("Mock ready endpoint called for player: " + request.player_name);
    
    nlohmann::json json = create_mock_game_state();
    return Result<nlohmann::json>(json);
});

ServerMethod<StateRequest> mock_state_method = ServerMethod<StateRequest>("/", HttpMethod::GET, 
[](const StateRequest& request) {
    Logger::instance().debug("Mock state endpoint called for player: " + request.player_name);
    
    nlohmann::json json = create_mock_game_state_with_game();
    return Result<nlohmann::json>(json);
});

ServerMethod<GuessRequest> mock_guess_method = ServerMethod<GuessRequest>("/guess", HttpMethod::POST, 
[](const GuessRequest& request) {
    Logger::instance().debug("Mock guess endpoint called for player: " + request.player_name + " with guess: " + request.guess);
    
    // Return mock game state with updated round
    nlohmann::json json = create_mock_game_state_with_game();
    
    return Result<nlohmann::json>(json);
});

