#include <string>
#include <vector>
#include "server/utils/result.h"

#include "nlohmann/json.hpp"

class Vote{
    private:
        std::string player_name;
        std::vector<std::string> votes_for;
        std::vector<std::string> votes_against;

    public:

        Vote(std::string voted_player = "");
        void vote_for(std::string voting_player);
        void vote_against(std::string voting_player);

        bool get_result() const;

        std::string get_player_name() const{
            return player_name;
        }
        bool is_vote_ended(int player_count) const;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vote, player_name, votes_for, votes_against);
};