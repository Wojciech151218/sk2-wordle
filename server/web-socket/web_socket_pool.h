#pragma once

#include <vector>
#include "nlohmann/json.hpp"
#include "server/utils/global_state.h"
#include "server/server/tcp_socket.h"

class WebSocketPool : public GlobalState<WebSocketPool> {
    private:
        std::unordered_map<int, std::reference_wrapper<TcpSocket>>  connections;

    public:
        WebSocketPool();
        ~WebSocketPool();
        
        void set_connections(std::unordered_map<int, std::reference_wrapper<TcpSocket>>  connections);
        void broadcast_all(const nlohmann::json& json);
        bool is_socket_connected(const TcpSocket& socket) const;
};