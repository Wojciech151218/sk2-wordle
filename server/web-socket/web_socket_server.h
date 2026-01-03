#include "server/server/tcp_server.h"
#include "server/web-socket/web_socket_pool.h"


class WebSocketServer : public TcpServer{


    protected:
        void on_client_connected(TcpSocket& client_socket) override;
        Result<std::string> handle_message(TcpSocket& socket, std::string message) override;

    public:
        void start(int port, std::string address);
        WebSocketServer();

};