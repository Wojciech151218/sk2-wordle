#include "server/tcp_server.h"
#include "server/web-socket/web_socket_pool.h"


class WebSocketServer : public TcpServer{
    private: 
        WebSocketPool& web_socket_pool;

    protected:
        void handle_client(TcpSocket* socket) override;
        void run_loop() override;


    public:
        WebSocketServer();


};