#include "server/web-socket/web_socket_pool.h"

WebSocketPool::WebSocketPool()  {
}

WebSocketPool::~WebSocketPool() {
}


void WebSocketPool::add(const WebSocketConnection& connection) {
    atomic([&]() {
        auto logger = &Logger::instance();
        logger->debug("Adding connection to pool " + connection.get_info());
        connections.push_back(connection);
    });
}

void WebSocketPool::remove(const WebSocketConnection& connection) {
    atomic([&]() {
        auto logger = &Logger::instance();
        logger->debug("Removing connection from pool " + connection.get_info());
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

void WebSocketPool::remove(const TcpSocket& connection_socket) {
    atomic([&]() {
        auto logger = &Logger::instance();
        logger->debug("Removing connection from pool " + connection_socket.get_host().value() + ":" + std::to_string(connection_socket.get_port().value()));
        connections.erase(
            std::remove(
                connections.begin(),
                connections.end(),
                connection_socket
            ),
            connections.end()
        );   
    });
}

void WebSocketPool::broadcast_all(const nlohmann::json& json) {
    atomic([&]() {
        auto logger = &Logger::instance();
        for (auto& connection : connections) {
                connection.send(json).log_debug<void*>();
                logger->debug("Broadcasted to connection: " + json.dump());
        }
    });
}

bool WebSocketPool::is_socket_connected(const TcpSocket& socket) const {
    atomic([&]() {
        return std::any_of(
            connections.begin(),
            connections.end(),
            [&](const WebSocketConnection& connection) {
                return connection.get_socket() == socket;
            }
        );
    });
}