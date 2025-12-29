#include "server/server/tcp_server.h"
#include "server/utils/logger.h"
#include <string>
#include <chrono>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>

TcpServer::TcpServer()
    :
      thread_pool(10, [this](TcpSocket& client) { handle_client_task(client); }),
      client_timeout(std::chrono::seconds(30)),
      epoll_fd(epoll_create1(0))
    {
        if (epoll_fd == -1) {
            logger.error(Error(std::string("Failed to create epoll instance")));
            std::runtime_error("Failed to create epoll instance");
        }

    }

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
    connections.erase(socket.get_fd());
    return Result<bool>(true);
}



bool TcpServer::stop_task_condition(TcpSocket& socket) {
    return socket.get_connection_state() == TcpSocket::ConnectionState::CLOSING
    || socket.get_connection_state() == TcpSocket::ConnectionState::IDLE;
}

void TcpServer::handle_client_task(TcpSocket& client_socket) {

    Result<bool> result = Result<bool>(Error("Invalid connection state"));
    do {
        switch (client_socket.get_connection_state()) {
            case TcpSocket::ConnectionState::CONNECTED:
                result = handle_connected(client_socket);
                break;
            case TcpSocket::ConnectionState::WRITING:
                result = handle_writing(client_socket);
                break;
            case TcpSocket::ConnectionState::IDLE:
                result = handle_idle(client_socket);
                break;
            case TcpSocket::ConnectionState::READING:
                result = handle_reading(client_socket);
                break;
            case TcpSocket::ConnectionState::CLOSING:
                result = handle_closing(client_socket);
                break;
        }

        if (result.log_error().is_err()) {
            handle_closing(client_socket);
            return;
        }
        if (result.unwrap()) {
            handle_state_change(client_socket);
        }
    } while (!stop_task_condition(client_socket));
}


void TcpServer::handle_server_event() {
    auto accept_result = server_socket.accept();
    if (accept_result.is_err()) {
        logger.error("Failed to accept connection");
        accept_result.log_error();
        return;
    }

    std::unique_ptr<TcpSocket> client_socket = std::make_unique<TcpSocket>(accept_result.unwrap());
    client_socket->set_connection_state(TcpSocket::ConnectionState::CONNECTED);
    connections.emplace(client_socket->get_fd(), *client_socket);

    // Add client socket to epoll with edge-triggered mode
    struct epoll_event client_ev;
    client_ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    client_ev.data.fd = client_socket->get_fd();
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket->get_fd(), &client_ev) == -1) {
        logger.error(Error(std::string("Failed to add client socket to epoll")));
        client_socket.reset();
        connections.erase(client_socket->get_fd());
        return;
    }
    
    return;
}

void TcpServer::handle_client_event(TcpSocket& client_socket) {
    int client_fd = client_socket.get_fd();
    if (events.at(client_fd).events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
        client_socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
    }
    thread_pool.enqueue(client_socket);

    // Check if this is a close event (this will be determined by connection state or error conditions)
    if (client_socket.get_connection_state() == TcpSocket::ConnectionState::CLOSING) {
        logger.debug("Connection closed");
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
        connections.erase(client_fd);
        return;
    }

    return;
}

void TcpServer::run() {
    if (server_thread.joinable()) {
        Logger::instance().info("Server is already running");
        return;
    }
    
    server_thread = std::thread(&TcpServer::run_loop, this);
}


void TcpServer::run_loop() {

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_socket.get_fd();

    if(Result<int>::from_bsd(
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket.get_fd(), &ev),
        "Failed to add server socket to epoll"
    ).log_error().is_err()) {
        close(epoll_fd);
        return;
    }

    while (true) {
        int nfds = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
        if(Result<int>::from_bsd(
            nfds,
            "Failed to wait for events"
        ).log_error().is_err()) {
            break;
        }
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            auto it = connections.find(fd);
            if (it == connections.end()) {
                logger.warn("Received event for unknown socket fd: " + std::to_string(fd));
                continue;
            }
            TcpSocket& socket = it->second;
            
            if (socket == server_socket) handle_server_event();
            else handle_client_event(socket);    
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