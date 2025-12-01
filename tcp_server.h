#pragma once

#include "tcp_socket.h"
#include "Result.h"

class TcpServer {
  private:
    TcpSocket socket;

  public:
    TcpServer();
    ~TcpServer();

    void run(int port, std::string address);
    void stop();
    Result<std::string> respond(const std::string& data);
};