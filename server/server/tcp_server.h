#pragma once

#include "server/server/tcp_socket.h"
#include "server/server/thread_pool.h"

#include <array>
#include <chrono>
#include <memory>
#include <sys/epoll.h>
#include <thread>
#include <vector>
#include <memory>
#define MAX_EVENTS 64

class TcpServer {
  protected:
    TcpSocket server_socket;
    ThreadPool thread_pool;
    std::chrono::milliseconds client_timeout;
    std::thread server_thread;
    int epoll_fd;
    std::array<struct epoll_event, MAX_EVENTS> events;
    std::unordered_map<int, std::reference_wrapper<TcpSocket>> connections;
    Logger& logger = Logger::instance();
    void run_loop();
    virtual bool stop_task_condition(TcpSocket& socket);
    void handle_client_task(TcpSocket& socket);
    virtual void handle_state_change(TcpSocket& socket) = 0;

    virtual Result<bool> handle_connected(TcpSocket& socket);
    virtual Result<bool> handle_idle(TcpSocket& socket);
    virtual Result<bool> handle_writing(TcpSocket& socket);
    virtual Result<bool> handle_reading(TcpSocket& socket);
    virtual Result<bool> handle_closing(TcpSocket& socket);


    void handle_server_event();
    void handle_client_event(TcpSocket& socket);

    std::vector<std::reference_wrapper<TcpSocket>>::iterator find_client_socket(int fd);

    



  public:
    TcpServer();
    virtual ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;

};