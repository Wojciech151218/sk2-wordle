#pragma once

#include "server/tcp_socket.h"
#include "utils/logger.h"
#include "utils/result.h"

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