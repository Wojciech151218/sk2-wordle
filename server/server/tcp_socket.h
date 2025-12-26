#pragma once

#include "server/utils/result.h"

#include <chrono>
#include <optional>
#include <string>

class TcpSocket {
  public: 
  enum class ConnectionState {
    IDLE,
    WRITING,
    READING,
    CLOSING
  };
  private:
    
    int socket_fd;
    std::optional<std::string> host;
    std::optional<int> port;
    std::chrono::steady_clock::time_point last_activity;

    std::string recv_buffer;
    std::string send_buffer;


    Result<int> check_connected(std::string message) const;
    void touch();

    ConnectionState connection_state = ConnectionState::IDLE;
  public:

    ConnectionState get_connection_state() const {
      return connection_state;
    }
    void set_connection_state(ConnectionState state) {
      connection_state = state;
    }
    TcpSocket();
    TcpSocket(int socket_fd, const std::string& host, int port);

    Result<TcpSocket> listen(const std::string& host, int port, int max_connections = 10);
    //Result<TcpSocket> connect(const std::string& host, int port);
    Result<void*> disconnect();

    void set_send_buffer(std::string data);
    Result<bool> send();
    
    Result<bool> receive(std::function<bool(std::string)> protocol_callback);
    std::string flush_recv();

    
    Result<TcpSocket> accept();

    std::optional<std::string> get_host() const;
    std::optional<int> get_port() const;
    int get_fd() const;

    std::chrono::milliseconds time_since_last_activity() const;
    bool should_timeout(const std::chrono::milliseconds& timeout) const {
      return time_since_last_activity() > timeout;
    }

    bool operator==(const TcpSocket& other) const {
        return this->socket_fd == other.socket_fd;
    }
};