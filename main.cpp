
#include <cstdlib>
#include <iostream>
#include <string>

#include "server/utils/logger.h"
#include "server/http/http_server.h"
#include "logic/endpoints/endpoints.h"
#include "server/utils/config.h"
#include <memory>
#include "server/web-socket/web_socket_server.h"
#include "server/cron/cron.h"
#include "logic/endpoints/endpoints.h"
#include "logic/mocks/mock_endpoints.h"
using namespace std;




void keep_alive() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    Config& config = Config::instance();
    config.load_config();

    bool mock = config.get_config("mock").value() == "true";
    if(mock) {
        Logger::instance().warn("running in mock mode");

    }

    auto game_cron = mock ? get_mock_game_cron() : get_game_cron();
    game_cron->start();

    HttpServer server;
    if(mock) {
        server.add_method(mock_join_method);
        server.add_method(mock_ready_method);
        server.add_method(mock_leave_method);
        server.add_method(mock_state_method);
        server.add_method(mock_guess_method);
    }else{
        server.add_method(join_method);
        server.add_method(ready_method);
        server.add_method(leave_method);
        server.add_method(state_method);
        server.add_method(guess_method);
    }
    server.start(
        std::stoi(config.get_config("http_port").value()), 
        config.get_config("address").value()
    );
    server.run();

    WebSocketServer web_socket_server;
    web_socket_server.start(
        std::stoi(config.get_config("websocket_port").value()), 
        config.get_config("address").value()
    );
    web_socket_server.run();
    keep_alive();

    return 0;
}