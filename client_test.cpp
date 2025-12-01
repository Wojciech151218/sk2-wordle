#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "logger.h"
#include "tcp_client.h"

namespace {
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <address> <port> <message>\n";
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
    if (argc != 4) {
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
    std::string message = argv[3];

    configure_logger();
    Logger& logger = Logger::instance();
    logger.debug("Client test harness starting");

    TcpClient client;

    auto connect_result = client.connect(address, port);
    if (connect_result.is_err()) {
        logger.error(connect_result.unwrap_err());
        return 1;
    }

    auto response = client.request(message);
    if (response.is_err()) {
        logger.error(response.unwrap_err());
        client.disconnect();
        return 1;
    }

    logger.info("Received from server: " + response.unwrap());

    auto disconnect_result = client.disconnect();
    if (disconnect_result.is_err()) {
        logger.error(disconnect_result.unwrap_err());
    }

    logger.debug("Client test harness stopped");
    return 0;
}

