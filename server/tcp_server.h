#pragma once

#include "server/tcp_socket.h"
#include "server/thread_pool.h"
#include "server/router.h"

class TcpServer {
  private:
    TcpSocket socket;
    ThreadPool thread_pool;
    Router router;
    //std::vector<ServerMethod> server_methods;
  public:
    TcpServer();
    ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    void handle_client(TcpSocket* socket);
    void add_method(const ServerMethod & method);
};