class Player {
    private:
        std::string player_name;    // Nazwa gracza
        int round_errors;           // Liczba błędów w rundzie
        int all_errors;             // Łączna liczba błędów
        bool is_alive;              // Czy gracz żyje


    public:
        Player(const std::string& name);
            //: player_name(name), round_errors(0), all_errors(0), is_alive(true) {}


        void reset_state();//{
        //     round_errors = 0;
        //     all_errors = 0;
        //     is_alive = true;
        // }

        void get_is_alive();

        void handle_round(bool status, int errors);//{
            //         player.all_errors += player.round_errors;  // Dodajemy błędy z tej rundy do całkowitych błędów
            // if (player.is_active && player.round_errors >= 6) {  // Jeśli gracz ma 6 błędów, zostaje wyeliminowany
            //     player.is_alive = false;
            // }
            // player.round_errors = 0;  // Resetujemy liczbę błędów w tej rundzie}

        void set_is_alive(bool status);

        void set_round_errors(int errors);


}