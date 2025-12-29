#include "server/server/tcp_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <fcntl.h>


TcpSocket::TcpSocket()
    : socket_fd(socket(AF_INET, SOCK_STREAM, 0)),
      last_activity(std::chrono::steady_clock::now()) {

    Result<int>::from_bsd(
        socket_fd,
        "Failed to create socket"
    ).log_error();

    // int flags = fcntl(socket_fd, F_GETFL, 0);
    // fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

TcpSocket::TcpSocket(int socket_fd, const std::string& host, int port)
    :  host(host),
       port(port),
       socket_fd(socket_fd),
       last_activity(std::chrono::steady_clock::now()) {

    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // int flags = fcntl(socket_fd, F_GETFL, 0);
    // fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

}


Result<int> TcpSocket::check_connected(std::string message) const {
    return Result<int>::from_bsd(
        socket_fd,
        message
    );
}

Result<TcpSocket> TcpSocket::listen(const std::string& host, int port, int max_connections) {
    this->host = host;
    this->port = port;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());


    return check_connected("Failed to create socket")
        .chain_from_bsd(
            bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)), 
            "Failed to create socket"
        )
        .chain_from_bsd(
            ::listen(socket_fd, max_connections), 
            "Failed to listen on socket"
        )
        .finally<TcpSocket>([&]() {
            return *this;
        });
}


Result<void*> TcpSocket::disconnect() {
    return check_connected("Socket not connected")
    .chain_from_bsd(
        close(this->socket_fd),
    "Failed to close socket"
    )
    .finally<void*>([&]() {
        this->socket_fd = 0;
        host.reset();
        port.reset();
        return nullptr;
    });
}

void TcpSocket::set_send_buffer(std::string data) {
    send_buffer = data;
}

Result<bool> TcpSocket::send() {
 
    
    return check_connected("Socket not connected")
    .chain_from_bsd(
        ::send(this->socket_fd, send_buffer.c_str(), send_buffer.size(), 0), 
        "Failed to send data"
    ).finally<bool>([&](int bytes_sent) {
        touch();
        send_buffer.erase(0, bytes_sent);
        return send_buffer.empty();
    });
    
}

Result<bool> TcpSocket::receive() {
    char buffer[1024];
   

    return check_connected("Socket not connected")
    .chain_from_bsd(
        ::recv(this->socket_fd, buffer, 1024, 0), 
        "Failed to send data"
    )
    .chain<bool>([&](size_t result) {
        recv_buffer.append(buffer, result);
        if (!protocol_callback) {
            return Result<bool>(Error("Protocol callback not set"));
        }
        return Result<bool>(protocol_callback(recv_buffer));
    });
}

std::string TcpSocket::flush_recv() {
    auto data = recv_buffer;
    recv_buffer.clear();
    return data;
}

Result<TcpSocket> TcpSocket::accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    return check_connected("Socket not connected")
    .chain_from_bsd(
        ::accept(
            this->socket_fd,
            (struct sockaddr*)&client_addr,
            &client_addr_len
            ), 
        "Failed to accept socket"
    )
    .finally<TcpSocket>([&](int client_fd) {
        return TcpSocket(client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    });
}

void TcpSocket::touch() {
    last_activity = std::chrono::steady_clock::now();
}

std::chrono::milliseconds TcpSocket::time_since_last_activity() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - last_activity
    );
}

int TcpSocket::get_fd() const {
    return socket_fd;
}
std::optional<std::string> TcpSocket::get_host() const {
    return host;
}

std::optional<int> TcpSocket::get_port() const {
    return port;
}

std::string TcpSocket::socket_info() const {
    return  host.value_or("unknown") + ":" + std::to_string(port.value_or(0));
}