#pragma once

#include "server/utils/result.h"

#include <chrono>
#include <optional>
#include <string>

class TcpSocket {
  private:
    int socket_fd;
    std::optional<std::string> host;
    std::optional<int> port;
    std::chrono::steady_clock::time_point last_activity;
    std::function<std::optional<std::string>(std::string)> protocol_callback;
    std::unordered_map<std::string, std::string> metadata;
    bool is_half_closed = false;
  

    std::string recv_buffer;
    std::string send_buffer;


    Result<int> check_connected(std::string message) const;
    void touch();

  
  public:

    
      

    void set_protocol_callback(std::function<std::optional<std::string>(std::string)> callback) {
      protocol_callback = callback;
    }
    void set_half_closed() {
      is_half_closed = true;
    }
    bool get_half_closed() const {
      return is_half_closed;
    }

    void set_metadata(const std::string& key, const std::string& value) {
      metadata[key] = value;
    }
    std::optional<std::string> get_metadata(const std::string& key) const {
      return metadata.find(key) != metadata.end() ? std::optional<std::string>(metadata.at(key)) : std::nullopt;
    }

    TcpSocket();
    TcpSocket(int socket_fd, const std::string& host, int port);

    Result<TcpSocket> listen(const std::string& host, int port, int max_connections = 10);
    //Result<TcpSocket> connect(const std::string& host, int port);
    Result<void*> disconnect();

    void set_send_buffer(std::string data);
    Result<bool> send();
    
    Result<bool> receive();
    std::vector<std::string> flush_messages();

    void drain_buffer();

    
    Result<TcpSocket> accept();

    std::optional<std::string> get_host() const;
    std::optional<int> get_port() const;
    int get_fd() const;

    std::chrono::milliseconds time_since_last_activity() const;
    bool should_timeout(const std::chrono::milliseconds& timeout) const {
      return time_since_last_activity() > timeout ;
    }

    bool operator==(const TcpSocket& other) const {
        return this->socket_fd == other.socket_fd;
    }
    std::string socket_info() const;
};