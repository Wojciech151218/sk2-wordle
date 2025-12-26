#pragma once

#include "server/tcp_socket.h"
#include <vector>
#include "server/thread_pool.h"
#include "nlohmann/json.hpp"

class WebSocketPool : public GlobalState<WebSocketPool> {
    private:
        std::vector<TcpSocket> connections;

    public:
        WebSocketPool();
        ~WebSocketPool();
        
        void add(const TcpSocket& connection);
        void remove(const TcpSocket& connection);
        void broadcast_all(const nlohmann::json& json);
        bool is_socket_connected(const TcpSocket& socket) const;
};