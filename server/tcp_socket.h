#pragma once

#include "utils/result.h"

#include <chrono>
#include <optional>
#include <string>

class TcpSocket {
  private:
    int socket_fd;
    std::optional<std::string> host;
    std::optional<int> port;
    std::chrono::steady_clock::time_point last_activity;

    Result<int> check_connected(std::string message) const;
    void touch();

  public:
    TcpSocket();
    TcpSocket(int socket_fd, const std::string& host, int port);

    Result<TcpSocket> listen(const std::string& host, int port, int max_connections = 10);
    Result<TcpSocket> connect(const std::string& host, int port);
    Result<void*> disconnect();
    Result<size_t> send(const std::string& data);
    Result<std::string> receive(std::optional<std::chrono::milliseconds> timeout = std::nullopt);
    Result<TcpSocket> accept();

    std::optional<std::string> get_host() const;
    std::optional<int> get_port() const;

    std::chrono::milliseconds time_since_last_activity() const;
};