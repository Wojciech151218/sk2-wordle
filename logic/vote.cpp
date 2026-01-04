#include "vote.h"

Vote::Vote(std::string player_name)
    : player_name(player_name), votes_for(), votes_against() {}

void Vote::vote_for(std::string player_name) {
    votes_for.push_back(player_name);
}

void Vote::vote_against(std::string player_name) {
    votes_against.push_back(player_name);
}

bool Vote::get_result() const {
    return votes_for.size() > votes_against.size();
}