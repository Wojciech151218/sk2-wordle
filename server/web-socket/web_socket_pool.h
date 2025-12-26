#pragma once

#include <vector>
#include "nlohmann/json.hpp"
#include "server/utils/global_state.h"
#include "server/server/tcp_socket.h"

class WebSocketPool : public GlobalState<WebSocketPool> {
    private:
        std::vector<TcpSocket*> * connections;

    public:
        WebSocketPool(std::vector<TcpSocket*> * connections = nullptr);
        ~WebSocketPool();
        
        void set_connections(std::vector<TcpSocket*> * connections);
        void broadcast_all(const nlohmann::json& json);
        bool is_socket_connected(const TcpSocket& socket) const;
};