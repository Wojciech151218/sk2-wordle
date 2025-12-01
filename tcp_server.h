#pragma once

#include "tcp_socket.h"
#include "Result.h"
#include "thread_pool.h"

class TcpServer {
  private:
    TcpSocket socket;
    ThreadPool thread_pool;
  public:
    TcpServer();
    ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    void handle_client(TcpSocket* socket);
};