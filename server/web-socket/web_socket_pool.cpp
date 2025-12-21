#include "server/web-socket/web_socket_pool.h"

WebSocketPool::WebSocketPool() {
}

WebSocketPool::~WebSocketPool() {
}

void WebSocketPool::add(const WebSocketConnection& connection) {
    atomic([&]() {
        connections.push_back(connection);
    });
}

void WebSocketPool::remove(const WebSocketConnection& connection) {
    atomic([&]() {
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
        for (auto& connection : connections) {
                connection.send(json).log_debug<void*>();
        }
    });
}