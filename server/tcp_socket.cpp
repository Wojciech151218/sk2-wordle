#include "server/tcp_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>

TcpSocket::TcpSocket()
    : socket_fd(socket(AF_INET, SOCK_STREAM, 0)),
      last_activity(std::chrono::steady_clock::now()) {

    Result<int>::from_bsd(
        socket_fd,
        "Failed to create socket"
    ).log_error<int>();

    
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
}


Result<int> TcpSocket::check_connected(std::string message) const {
    return Result<int>::from_bsd(
        socket_fd,
        message
    ).log_error<int>();
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
        })
        .log_error<TcpSocket>();
}

Result<TcpSocket> TcpSocket::connect(const std::string& host, int port) {
    this->host = host;
    this->port = port;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);


    return check_connected("Failed to create socket")
        .chain_from_bsd(
        ::inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr),
        "Failed to convert host to IP address"
        )
        .chain_from_bsd(
            ::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)), 
            "Failed to connect to socket"
        )
        .finally<TcpSocket>([&]() {
            return *this;
        })
        .log_error<TcpSocket>();
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
    })
    .log_error<void*>();
}

Result<size_t> TcpSocket::send(const std::string& data) {
    return check_connected("Socket not connected")
    .chain_from_bsd(
        ::send(this->socket_fd, data.c_str(), data.size(), 0), 
        "Failed to send data"
    ).finally<size_t>([&](size_t result) {
        touch();
        return result;
    })
    .log_error<size_t>();

    
}

Result<std::string> TcpSocket::receive(std::optional<std::chrono::milliseconds> timeout) {
    char buffer[1024];

    return check_connected("Socket not connected")
    .chain<std::string>([&](auto) {
        while (true) {
            if (timeout.has_value()) {
                struct pollfd fd;
                fd.fd = this->socket_fd;
                fd.events = POLLIN;
                int poll_result = ::poll(&fd, 1, static_cast<int>(timeout->count()));
                if (poll_result == 0) {
                    return Result<std::string>(Error("Receive timeout"))
                        .log_warn<std::string>();
                }
                if (poll_result < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    return Result<std::string>(Error("Failed to wait for data"))
                        .log_error<std::string>();
                }
            }

            int bytes_read = ::recv(this->socket_fd, buffer, sizeof(buffer), 0);
            if (bytes_read < 0) {
                if (errno == EINTR) {
                    continue;
                }
                return Result<std::string>(Error("Failed to receive data"))
                    .log_error<std::string>();
            }
            if (bytes_read == 0) {
                return Result<std::string>(Error("Client disconnected"))
                    .log_warn<std::string>();
            }
            touch();
            return Result<std::string>(std::string(buffer, bytes_read));
        }
    });
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

std::optional<std::string> TcpSocket::get_host() const {
    return host;
}

std::optional<int> TcpSocket::get_port() const {
    return port;
}