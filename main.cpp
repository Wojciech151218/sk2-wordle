#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "logger.h"
#include "tcp_server.h"
#include "tcp_client.h"
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

    Logger& logger = Logger::instance();
    Logger::Options options{};
    options.info_enabled = true;
    options.debug_enabled = true;
    options.error_enabled = true;
    options.use_colors = true;
    logger.configure(options);
    logger.debug("Logger configured with defaults");

    TcpServer server;
    TcpClient client;
    std::atomic<bool> running{true};

    auto server_task = [&]() {
        server.run(port, address);
        while (running.load()) {
            auto response = server.respond("Hello, client!");
            if (response.is_err()) {
                if (running.load()) {
                    logger.error(response.unwrap_err());
                }
                break;
            }
            logger.info("Response sent to client: " + response.unwrap());
        }
    };

    auto client_task = [&]() {
        auto connect_result = client.connect(address, port);
        if (connect_result.is_err()) {
            logger.error(connect_result.unwrap_err());
            running.store(false);
            return;
        }

        auto request_result = client.request("Hello, server!");
        if (request_result.is_err()) {
            logger.error(request_result.unwrap_err());
        } else {
            logger.info("Server replied to client: " + request_result.unwrap());
        }

        auto disconnect_result = client.disconnect();
        if (disconnect_result.is_err()) {
            logger.error(disconnect_result.unwrap_err());
        }

        running.store(false);
    };

    std::thread server_thread(server_task);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread client_thread(client_task);

    client_thread.join();
    running.store(false);
    server.stop();
    server_thread.join();

    return 0;
}