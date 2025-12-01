#pragma once

#include "Result.h"

#include <optional>
#include <string>

class TcpSocket {
  private:
    int socket_fd;
    std::optional<std::string> host;
    std::optional<int> port;

    Result<int> check_connected(std::string message) const;

  public:
    TcpSocket();
    TcpSocket(int socket_fd, const std::string& host, int port);

    Result<TcpSocket> listen(const std::string& host, int port, int max_connections = 10);
    Result<TcpSocket> connect(const std::string& host, int port);
    Result<void*> disconnect();
    Result<size_t> send(const std::string& data);
    Result<std::string> receive(const TcpSocket& socket);
    Result<TcpSocket> accept();
};