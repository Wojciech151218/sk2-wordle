
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

#include "utils/logger.h"
#include "server/tcp_server.h"
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
    
    server.start(port, address);
    server.run();
    server.stop();

    return 0;
}