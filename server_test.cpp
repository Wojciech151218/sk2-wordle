#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "logger.h"
#include "tcp_server.h"

namespace {
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <address> <port> [response_message]\n";
}

int parse_port(const char* value) {
    try {
        int parsed = std::stoi(value);
        if (parsed <= 0 || parsed > 65535) {
            throw std::out_of_range("port out of range");
        }
        return parsed;
    } catch (const std::exception& ex) {
        throw std::invalid_argument(std::string("Invalid port value: ") + ex.what());
    }
}

void configure_logger() {
    Logger& logger = Logger::instance();
    Logger::Options options{};
    options.info_enabled = true;
    options.debug_enabled = true;
    options.error_enabled = true;
    options.use_colors = true;
    logger.configure(options);
}
}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        print_usage(argv[0]);
        return 1;
    }

    int port;
    try {
        port = parse_port(argv[2]);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    std::string address = argv[1];
    std::string response_payload = (argc == 4) ? argv[3] : "Hello, client!";

    configure_logger();
    Logger& logger = Logger::instance();
    logger.debug("Server test harness starting");

    TcpServer server;
    server.start(port, address);

    while (true) {
        auto client_response = server.run(response_payload);
        if (client_response.is_err()) {
            logger.error(client_response.unwrap_err());
            break;
        }
        logger.info("Received from client: " + client_response.unwrap());
    }

    server.stop();
    logger.debug("Server test harness stopped");
    return 0;
}

