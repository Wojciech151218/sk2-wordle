class Game {
private:
    int round;                  // Numer rundy
    time_t round_end_time;      // Czas do końca rundy
    time_t round_duration;      // Czas trwania jednej rundy
    time_t game_start_time;     // Czas rozpoczęcia gry

    std::vector<Player> players_list; // Lista graczy
    std::vector<Round> rounds;        // Lista rund

public:
    Game(std::vector<Player> player, time_t round_duration);
    
    bool end_round();               // Zakończenie rundy
    bool start_round();             // Zakończenie rundy
    bool check_if_game_is_over();   // Sprawdzenie, czy gra się zakończyła

    int get_round() const;          // Pobranie numeru rundy
    bool is_game_active() const;    // Sprawdzenie, czy gra trwa

    void make_guess(std::string player_name, std::string guess);  //referencja do guesses 
};
