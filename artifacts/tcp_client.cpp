#include "artifacts/tcp_client.h"

#include <string>

TcpClient::TcpClient() : socket(TcpSocket()) {}

TcpClient::~TcpClient() {
    socket.disconnect();
}

Result<void*> TcpClient::connect(const std::string& host, int port) {
    logger.info("Connecting to server " + host + ":" + std::to_string(port));
    return socket.connect(host, port).finally<void*>([&]() {
        return nullptr;
    });
}

Result<void*> TcpClient::disconnect() {
    logger.info("Disconnecting from server");
    return socket.disconnect();
}

Result<std::string> TcpClient::request(const std::string& data) {
    logger.info("Sending request to server: " + data);
    return socket.send(data).and_then<std::string>([&](auto) {
        return socket.receive();
    });
}