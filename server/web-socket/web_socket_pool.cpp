#include "server/web-socket/web_socket_pool.h"
#include "server/web-socket/web_socket_frame.h"
#include "server/utils/logger.h"

void WebSocketPool::set_connections(std::unordered_map<int, TcpSocket>& connections) {
    this->connections = &connections;
}

void WebSocketPool::broadcast_all(const nlohmann::json& json) {
    if (!connections || connections->empty()) return;
    atomic([&]() {
        auto logger = &Logger::instance();
        for (auto& [fd, connection] : *connections) {
            auto frame = WebSocketFrame::text(json.dump());
            
            connection.set_send_buffer(frame.to_string());
            connection.set_connection_state(TcpSocket::ConnectionState::WRITING);
            connection.send();
            logger->info("Broadcasted to connection: " + connection.socket_info());
        }
    });
}

bool WebSocketPool::is_socket_connected(const TcpSocket& socket) const {
    if (!connections || connections->empty()) return false;
    return atomic([&]() {
        return connections->find(socket.get_fd()) != connections->end();
    });
}