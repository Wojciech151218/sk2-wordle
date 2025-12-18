// player.cpp
#include "player.h"
#include <iostream>

Player::Player(const std::string& name)
    : player_name(name), round_errors(0), all_errors(0), is_alive(true) {}

// Resetuje stan gracza na początkowy
void Player::reset_state() {  
    round_errors = 0;
    all_errors = 0;
    is_alive = true;
}

// Zwraca, czy gracz jest żywy
bool Player::get_is_alive() {
    return is_alive;
}

// Obsługuje zakończenie rundy dla gracza
void Player::handle_round() { //zamienione z : (bool status, int errors)
    all_errors += round_errors;

    if (is_alive && round_errors >= 6) {
        is_alive = false;
    }
    round_errors = 0;
}

// Ustawia, czy gracz jest żywy
void Player::set_is_alive(bool status) {
    is_alive = status;
}

// Ustawia liczbę błędów w rundzie
void Player::set_round_errors(int errors) {
    if(errors >= 0 && errors <= 6){
        round_errors = errors;
    }
    else{
        std::cerr << "Invalid number of errors. It must be between 0 and 6." << std::endl;
    }
}
