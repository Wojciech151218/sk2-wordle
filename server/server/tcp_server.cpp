#include "server/server/tcp_server.h"
#include "server/utils/logger.h"
#include <string>
#include <chrono>
#include <sys/epoll.h>
#include <cerrno>
#include <unistd.h>
#include <vector>

TcpServer::TcpServer()
    :
      thread_pool(10, [this](TcpSocket& client) { }),
      client_timeout(std::chrono::seconds(2)),
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

void TcpServer::run() {
    if (server_thread.joinable()) {
        Logger::instance().info("Server is already running");
        return;
    }
    
    server_thread = std::thread(&TcpServer::run_loop, this);
}




void TcpServer::handle_server_event() {
    auto accept_result = server_socket.accept();
    if (accept_result.log_error("Failed to accept connection").is_err()) return;

    TcpSocket client_socket = accept_result.unwrap();
    on_client_connected(client_socket);

    int client_fd = client_socket.get_fd();
    connections.emplace(client_fd, std::move(client_socket));
    struct epoll_event client_ev;
    client_ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    client_ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
        logger.error(Error("Failed to add client socket to epoll"));
        connections.erase(client_fd);
        return;
    }
    
}


void TcpServer::handle_client_event(int fd,uint32_t events) {

    auto it = connections.find(fd);
    if (it == connections.end()) {
        logger.warn("Received event for unknown socket fd: " + std::to_string(fd));
        return;
    }
    TcpSocket& client_socket = it->second;

    if (events & (EPOLLHUP | EPOLLERR)) {
        drain_and_close(client_socket);
        return;
    }
    
    if (events & EPOLLRDHUP) {
        mark_peer_half_closed(client_socket);
    }
    
    if (events & EPOLLIN) {
        read_until_eagain(client_socket);
    }
    
    if (events & EPOLLOUT) {
        write_until_eagain(client_socket);
    }
}


void TcpServer::drain_and_close(TcpSocket& client_socket) {
    logger.debug("Draining and closing client " + client_socket.socket_info());

    auto messages = client_socket.flush_messages();
    for (auto& message : messages) {
        handle_message(client_socket, message);
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket.get_fd(), nullptr);
    connections.erase(client_socket.get_fd());
    client_socket.disconnect();
}

void TcpServer::mark_peer_half_closed(TcpSocket& client_socket) {
    logger.debug("Marking peer half closed for client " + client_socket.socket_info());

    client_socket.set_half_closed();
}

void TcpServer::read_until_eagain(TcpSocket& client_socket) {
    logger.debug("Reading data from client " + client_socket.socket_info());

    auto receive_result = client_socket.receive();

    
    if(receive_result.log_error("Failed to read data").is_err()) {
        handle_error(client_socket);
        return;
    }
    if(receive_result.unwrap()) {
        client_socket.set_half_closed();
    }
    auto messages = client_socket.flush_messages();
    for (auto& message : messages) {
        handle_message(client_socket, message);
    }


}

void TcpServer::write_until_eagain(TcpSocket& client_socket) {
    logger.debug("Writing data to client " + client_socket.socket_info());

    auto send_result = client_socket.send();
    if (send_result.log_error("Failed to send data").is_err()) {
        handle_error(client_socket);
        return;
    }
    if(send_result.unwrap() && client_socket.get_half_closed()) {
        client_socket.disconnect();
    }
}

void TcpServer::handle_error(TcpSocket& client_socket) {
    client_socket.disconnect();
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
        int nfds = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, 1000); // 1s wakeup for idle check
        if (nfds == -1) {
            if (errno == EINTR) continue;
            logger.error(Error(std::string("Failed to wait for events")));
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
      
            if (fd == server_socket.get_fd()) handle_server_event();
            else handle_client_event(fd,ev);    
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