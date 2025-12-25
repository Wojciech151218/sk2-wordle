#include "server/tcp_socket.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"

#include "server/web-socket/web_socket_frame.h"
#include "nlohmann/json.hpp"

class WebSocketConnection {
  private:
    TcpSocket socket;

    public:
    WebSocketConnection(TcpSocket socket);
    std::string get_info() const;
    static Result<WebSocketConnection> accept(TcpSocket& socket,HttpRequest& request);

    Result<void*> send(const nlohmann::json& json);
    Result<WebSocketFrame> receive();
    Result<void*> close();
    bool operator==(const WebSocketConnection& other) const {
        return this->socket == other.socket;
    }
    TcpSocket get_socket() const {
        return socket;
    }
};