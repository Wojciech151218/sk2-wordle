#pragma once

#include "server/server/tcp_socket.h"
#include "server/server/thread_pool.h"

#include <array>
#include <chrono>
#include <sys/epoll.h>
#include <thread>
#include <unordered_map>
#define MAX_EVENTS 64

class TcpServer {
  protected:
    TcpSocket server_socket;
    ThreadPool thread_pool;
    std::chrono::milliseconds client_timeout;
    std::thread server_thread;
    int epoll_fd;
    std::array<struct epoll_event, MAX_EVENTS> events;
    std::unordered_map<int, TcpSocket> connections;
    Logger& logger = Logger::instance();
    void run_loop();
  

    virtual void handle_message(TcpSocket& socket, std::string message) = 0;


    virtual void drain_and_close(TcpSocket& client_socket);
    virtual void mark_peer_half_closed(TcpSocket& client_socket);
    virtual void read_until_eagain(TcpSocket& client_socket);
    virtual void write_until_eagain(TcpSocket& client_socket);
    virtual void handle_error(TcpSocket& client_socket);
    virtual void on_client_connected(TcpSocket& client_socket) = 0;

    void handle_server_event();
    void handle_client_event(int fd,uint32_t events);


    



  public:
    TcpServer();
    virtual ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;

};