#include "tcp_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

TcpSocket::TcpSocket() : socket_fd(socket(AF_INET, SOCK_STREAM, 0)) {}

TcpSocket::TcpSocket(int socket_fd, const std::string& host, int port)
    :  host(host), port(port),socket_fd(socket_fd) {
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
        .and_then_from_bsd(
            bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)), 
            "Failed to create socket"
        )
        .and_then_from_bsd(
            ::listen(socket_fd, max_connections), 
            "Failed to listen on socket"
        )
        .finally<TcpSocket>([&]() {
            return *this;
        });
}

Result<TcpSocket> TcpSocket::connect(const std::string& host, int port) {
    this->host = host;
    this->port = port;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);


    return check_connected("Failed to create socket")
        .and_then_from_bsd(
        ::inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr),
        "Failed to convert host to IP address"
        )
        .and_then_from_bsd(
            ::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)), 
            "Failed to connect to socket"
        )
        .finally<TcpSocket>([&]() {
            return *this;
        });
}

Result<void*> TcpSocket::disconnect() {
    return check_connected("Socket not connected")
    .and_then_from_bsd(
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

Result<size_t> TcpSocket::send(const std::string& data) {
    return check_connected("Socket not connected")
    .and_then_from_bsd(
        ::send(this->socket_fd, data.c_str(), data.size(), 0), 
        "Failed to send data"
    ).finally<size_t>([&](size_t result) {
        return result;
    });

    
}

Result<std::string> TcpSocket::receive() {
    char buffer[1024];

    return check_connected("Socket not connected")
    .and_then_from_bsd(
        ::recv(this->socket_fd, buffer, 1024, 0), 
        "Failed to receive data"
    )
    .finally<std::string>([&]() {
        return std::string(buffer);
    });
}

Result<TcpSocket> TcpSocket::accept() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    return check_connected("Socket not connected")
    .and_then_from_bsd(
        ::accept(
            this->socket_fd,
            (struct sockaddr*)&client_addr,
            &client_addr_len
            ), 
        "Failed to accept socket"
    )
    .finally<TcpSocket>([&](int client_fd) {
        return TcpSocket(client_fd, this->host.value(), this->port.value());
    });
}