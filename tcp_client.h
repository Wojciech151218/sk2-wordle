#pragma once

#include "tcp_socket.h"

#include <string>

class TcpClient {
  private:
    TcpSocket socket;
    Logger& logger = Logger::instance();

  public:
    TcpClient();
    ~TcpClient();

    Result<void*> connect(const std::string& host, int port);
    Result<void*> disconnect();
    Result<std::string> request(const std::string& data);
 
};