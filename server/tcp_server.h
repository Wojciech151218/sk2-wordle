#pragma once

#include "server/tcp_socket.h"
#include "server/thread_pool.h"
#include "server/router.h"

#include <chrono>

class TcpServer {
  private:
    TcpSocket socket;
    ThreadPool thread_pool;
    Router router;
    std::chrono::milliseconds client_timeout;



  public:
    TcpServer();
    ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    void handle_client(TcpSocket* socket);
    void add_method(const ServerMethod & method);
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;
};