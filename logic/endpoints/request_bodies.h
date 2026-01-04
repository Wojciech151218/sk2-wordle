
#pragma once

#include "server/http/request_body.h"
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


class VoteRequest : public RequestBody {
    public:
        VoteRequest(std::string voted_player, std::string voting_player,bool vote_for) :
         RequestBody(), voted_player(voted_player), voting_player(voting_player), vote_for(vote_for) {};
        VoteRequest() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) override;

        std::string voted_player;
        std::string voting_player;
        bool vote_for;
};

