#include <string>
#include <vector>

#include "nlohmann/json.hpp"

class Vote{
    private:
        std::string player_name;
        std::vector<std::string> votes_for;
        std::vector<std::string> votes_against;

    public:

        Vote(std::string player_name = "");
        void vote_for(std::string player_name);
        void vote_against(std::string player_name);

        bool get_result() const;

        std::string get_player_name() const{
            return player_name;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vote, player_name, votes_for, votes_against);
};