#pragma once

#include "server/tcp_socket.h"
#include "server/web-socket/web_socket_connection.h"
#include <vector>
#include "server/thread_pool.h"
#include "nlohmann/json.hpp"

class WebSocketPool : public GlobalState<WebSocketPool> {
    private:
        std::vector<WebSocketConnection> connections;

    public:
        WebSocketPool();
        ~WebSocketPool();
        
        void add(const WebSocketConnection& connection);
        void remove(const WebSocketConnection& connection);
        void remove(const TcpSocket& connection_socket);
        void broadcast_all(const nlohmann::json& json);
        bool is_socket_connected(const TcpSocket& socket) const;
};