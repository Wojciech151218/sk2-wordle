#include "vote.h"

Vote::Vote(std::string player_name)
    : voted_player(player_name), votes_for(), votes_against() {}

void Vote::vote_for(std::string voting_player) {
    votes_for.push_back(voting_player);
}

void Vote::vote_against(std::string voting_player) {
    votes_against.push_back(voting_player);
}

bool Vote::get_result() const {
    return votes_for.size() > votes_against.size();
}

bool Vote::is_vote_ended(int player_count) const {
    return votes_for.size() + votes_against.size() >= player_count  -1 ;
}