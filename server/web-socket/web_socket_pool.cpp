#include "server/web-socket/web_socket_pool.h"
#include "server/web-socket/web_socket_frame.h"

WebSocketPool::WebSocketPool(std::vector<TcpSocket*> * connections) : connections(connections) {
}

WebSocketPool::~WebSocketPool() {
}
    

void WebSocketPool::set_connections(std::vector<TcpSocket*> * connections) {
    this->connections = connections;
}

void WebSocketPool::broadcast_all(const nlohmann::json& json) {
    if (!connections) return;
    atomic([&]() {
        auto logger = &Logger::instance();
        for (auto& connection : *connections) {
            auto frame = WebSocketFrame::text(json.dump());
            
            connection->set_send_buffer(frame.to_string());
            connection->set_connection_state(TcpSocket::ConnectionState::WRITING);
            connection->send();
            logger->info("Broadcasted to connection: " + connection->socket_info());
        }
    });
}

bool WebSocketPool::is_socket_connected(const TcpSocket& socket) const {
    if (!connections) return false;
    return atomic([&]() {
        return std::any_of(
            connections->begin(),
            connections->end(),
            [&](TcpSocket* connection) {
                return connection != nullptr && *connection == socket;
            }
        );
    });
}