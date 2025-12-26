#include "server/server/tcp_server.h"
#include "server/utils/logger.h"
#include <string>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>

TcpServer::TcpServer()
    :
      thread_pool(10, [this](TcpSocket* client) { handle_client_task(client); }),
      client_timeout(std::chrono::seconds(30))
    {}

TcpServer::~TcpServer() {
    stop();
}


void TcpServer::start(int port, std::string address) {
    Logger& logger = Logger::instance();
    server_socket.listen(address, port)
        .finally<void*>([&]() {
            logger.info("Listening for incoming connections...");
            return nullptr;
        });
}

void TcpServer::stop() {
    if (!server_thread.joinable()) {
        return;
    }
    
    Logger::instance().info(
        "Stopping TCP server on " + 
        (server_socket.get_host().value_or("unknown") + 
        ":" + 
        std::to_string(server_socket.get_port().value_or(0)))
    );
    server_socket.disconnect();
    
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

Result<bool> TcpServer::handle_connected(TcpSocket& socket) {;
    return handle_reading(socket);
}

Result<bool> TcpServer::handle_idle(TcpSocket& socket) {
    return handle_reading(socket);
}

Result<bool> TcpServer::handle_writing(TcpSocket& socket) {
    return socket.send();
}

Result<bool> TcpServer::handle_reading(TcpSocket& socket) {
    return socket.receive();
}

Result<bool> TcpServer::handle_closing(TcpSocket& socket) {
    socket.disconnect();
    return Result<bool>(true);
}



bool TcpServer::stop_task_condition(TcpSocket& socket) {
    return socket.get_connection_state() == TcpSocket::ConnectionState::CLOSING
    || socket.get_connection_state() == TcpSocket::ConnectionState::IDLE;
}

void TcpServer::handle_client_task(TcpSocket* client_socket) {

    Result<bool> result = Result<bool>(Error("Invalid connection state"));
    do {
        switch (client_socket->get_connection_state()) {
            case TcpSocket::ConnectionState::CONNECTED:
                result = handle_connected(*client_socket);
                break;
            case TcpSocket::ConnectionState::WRITING:
                result = handle_writing(*client_socket);
                break;
            case TcpSocket::ConnectionState::IDLE:
                result = handle_idle(*client_socket);
                break;
            case TcpSocket::ConnectionState::READING:
                result = handle_reading(*client_socket);
                break;
            case TcpSocket::ConnectionState::CLOSING:
                result = handle_closing(*client_socket);
                break;
        }

        if (result.is_err()) {
            //handle_closing(client_socket);
            result.log_error();
            client_socket->disconnect().log_error();
            delete client_socket;
            return;
        }
        if (result.unwrap()) {
            handle_state_change(*client_socket);
        }
    } while (!stop_task_condition(*client_socket));
}


void TcpServer::run() {
    if (server_thread.joinable()) {
        Logger::instance().info("Server is already running");
        return;
    }
    
    server_thread = std::thread(&TcpServer::run_loop, this);
}


void TcpServer::run_loop() {
    Logger& logger = Logger::instance();

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        logger.error(Error(std::string("Failed to create epoll instance")));
        return;
    }

    // Add server socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_socket.get_fd();
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket.get_fd(), &ev) == -1) {
        logger.error(Error("Failed to add server socket to epoll"));
        close(epoll_fd);
        return;
    }

    const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            logger.error(Error("epoll_wait failed"));
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            // Handle server socket - accept new connection
            if (fd == server_socket.get_fd()) {
                auto accept_result = server_socket.accept();
                if (accept_result.is_err()) {
                    logger.error(Error(std::string("Failed to accept connection")));
                    continue;
                }

                TcpSocket* client_socket = new TcpSocket(accept_result.unwrap());
                client_socket->set_connection_state(TcpSocket::ConnectionState::CONNECTED);
                connections.push_back(client_socket);

                // Add client socket to epoll with edge-triggered mode
                struct epoll_event client_ev;
                client_ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
                client_ev.data.fd = client_socket->get_fd();
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket->get_fd(), &client_ev) == -1) {
                    logger.error(Error(std::string("Failed to add client socket to epoll")));
                    delete client_socket;
                    connections.pop_back();
                }
            }
            // Handle client socket events
            else {
                // Find the socket in connections
                auto it = std::find_if(connections.begin(), connections.end(), 
                    [fd](TcpSocket* sock) { return sock->get_fd() == fd; });

                if (it == connections.end()) {
                    logger.warn("Received event for unknown socket fd: " + std::to_string(fd));
                    continue;
                }

                TcpSocket* client_socket = *it;

                // Handle close events
                if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                    logger.debug("Connection closed " );
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    connections.erase(it);
                    client_socket->set_connection_state(TcpSocket::ConnectionState::CLOSING);
                    thread_pool.enqueue(client_socket);
                }
                // Handle read/write events
                else if (events[i].events & (EPOLLIN | EPOLLOUT)) {
                    thread_pool.enqueue(client_socket);
                }
            }
        }
    }

    close(epoll_fd);
}

void TcpServer::set_client_timeout(std::chrono::milliseconds timeout) {
    client_timeout = timeout;
}

std::chrono::milliseconds TcpServer::get_client_timeout() const {
    return client_timeout;
}