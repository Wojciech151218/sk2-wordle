
#pragma once

#include "server/request_body.h"
#include <string>

class JoinRequest : public RequestBody {

    public:
     
        JoinRequest(std::string player_name) : RequestBody(), player_name(player_name) {};
        JoinRequest() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) override;

        std::string player_name;
};

class StateRequest : public RequestBody {
    public:
        StateRequest(std::string player_name, std::time_t timestamp) 
        : RequestBody(), player_name(player_name), timestamp(timestamp) {};
        StateRequest() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) override;

        std::string player_name;
        std::time_t timestamp;
};

class GuessRequest : public StateRequest {
    public:
        GuessRequest(std::string player_name, std::time_t timestamp, std::string guess) 
        : StateRequest(player_name, timestamp), guess(guess) {};
        GuessRequest() : StateRequest() {};
        Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) override;

        std::string guess;
};


