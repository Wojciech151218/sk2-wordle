#include "server/server/tcp_server.h"
#include "server/web-socket/web_socket_pool.h"


class WebSocketServer : public TcpServer{


    protected:
        void handle_state_change(TcpSocket& socket) override;


    public:
        void start(int port, std::string address);
        WebSocketServer();

        Result<bool> handle_connected(TcpSocket& socket) override;
};