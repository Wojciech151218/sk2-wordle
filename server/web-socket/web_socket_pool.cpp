#include "server/web-socket/web_socket_pool.h"

WebSocketPool::WebSocketPool()  {
}

WebSocketPool::~WebSocketPool() {
}


void WebSocketPool::add(const TcpSocket& connection) {
    atomic([&]() {
        auto logger = &Logger::instance();
        logger->debug("Adding connection to pool ");
        connections.push_back(connection);
    });
}

void WebSocketPool::remove(const TcpSocket& connection) {
    atomic([&]() {
        auto logger = &Logger::instance();
        logger->debug("Removing connection from pool ");
        connections.erase(
            std::remove(
                connections.begin(),
                connections.end(),
                connection
            ),
            connections.end()
        );
        
    });
}

void WebSocketPool::broadcast_all(const nlohmann::json& json) {
    atomic([&]() {
        auto logger = &Logger::instance();
        for (auto& connection : connections) {
                //connection.send(json).log_debug();
                logger->debug("Broadcasted to connection: " + json.dump());
        }
    });
}

bool WebSocketPool::is_socket_connected(const TcpSocket& socket) const {
    atomic([&]() {
        return std::any_of(
            connections.begin(),
            connections.end(),
            [&](const TcpSocket& connection) {
                return connection == socket;
            }
        );
    });
}