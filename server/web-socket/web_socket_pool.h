#pragma once

#include "server/web-socket/web_socket_connection.h"
#include <vector>

class WebSocketPool : public GlobalState<WebSocketPool> {
    private:
        std::vector<WebSocketConnection> connections;

    public:
        WebSocketPool();
        ~WebSocketPool();
        
        void add(const WebSocketConnection& connection);
        void remove(const WebSocketConnection& connection);
        void broadcast_all(const nlohmann::json& json);
};