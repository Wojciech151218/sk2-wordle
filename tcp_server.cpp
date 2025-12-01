#include "tcp_server.h"

#include <cstddef>
#include <string>

TcpServer::TcpServer() : socket(TcpSocket()) {}

TcpServer::~TcpServer() {
    socket.disconnect();
}

void TcpServer::run(int port, std::string address) {
    Logger& logger = Logger::instance();
    logger.info("Starting TCP server on " + address + ":" + std::to_string(port));
    socket.listen(address, port)
        .finally<void*>([&]() {
            logger.debug("Listening for incoming connections...");
            return nullptr;
        });
}

void TcpServer::stop() {
    Logger::instance().info("Stopping TCP server");
    socket.disconnect();
}


Result<std::string> TcpServer::respond(const std::string& data) {
    Logger& logger = Logger::instance();

    return socket.accept()
    .and_then<std::string>([&](TcpSocket client_socket) {
        return client_socket.receive(client_socket)
            .and_then<std::string>([&](std::string response) {
                client_socket.send(data);
                return response;
            });
    });
}