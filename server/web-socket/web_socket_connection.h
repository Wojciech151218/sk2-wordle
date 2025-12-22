#include "server/tcp_socket.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"

#include "server/web-socket/web_socket_frame.h"

class WebSocketConnection {
  private:
    TcpSocket socket;

    public:
    WebSocketConnection(TcpSocket socket);

    static Result<WebSocketConnection> accept(TcpSocket& socket,HttpRequest& request);

    Result<void*> send(const nlohmann::json& json);
    Result<WebSocketFrame> receive();
    Result<void*> close();
    bool operator==(const WebSocketConnection& other) const {
        return this->socket == other.socket;
    }
   
};