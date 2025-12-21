
#include <cstdlib>
#include <iostream>
#include <string>

#include "server/utils/logger.h"
#include "server/tcp_server.h"
#include "logic/endpoints/endpoints.h"
#include "server/utils/config.h"
#include <memory>

using namespace std;



namespace {
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <address> <port> \n" << std::endl;
}
}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    std::string address = argv[1];
    int port = atoi(argv[2]);


    Config& config = Config::instance();
    config.set_allowed_origin("http://localhost:5173");

    Logger& logger = Logger::instance();
    Logger::Options options{};
    options.info_enabled = true;
    options.debug_enabled = false;
    options.error_enabled = true;
    options.use_colors = true;
    logger.configure(options);

    TcpServer server;
    server.add_method(join_method);
    server.add_method(ready_method);
    server.add_method(leave_method);
    server.add_method(state_method);
    server.add_method(guess_method);
    server.start(port, address);
    server.run();
    server.stop();

    return 0;
}