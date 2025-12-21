#pragma once

#include "server/tcp_socket.h"
#include "server/thread_pool.h"
#include "server/router.h"

#include <chrono>
#include "server/web-socket/web_socket_pool.h"

class TcpServer {
  private:
    TcpSocket socket;
    ThreadPool thread_pool;
    Router router;
    std::chrono::milliseconds client_timeout;
    WebSocketPool& web_socket_pool;



  public:
    TcpServer();
    ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    void handle_client(TcpSocket* socket);

    template <typename Body>
    void add_method(const ServerMethod<Body>& method) {
        router.add_method(method);
    }
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;
};