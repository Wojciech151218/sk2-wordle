#pragma once

#include "server/server/tcp_socket.h"
#include "server/server/thread_pool.h"

#include <chrono>
#include <thread>

class TcpServer {
  protected:
    TcpSocket server_socket;
    ThreadPool thread_pool;
    std::chrono::milliseconds client_timeout;
    std::thread server_thread;
    std::vector<TcpSocket*> connections;
    Logger& logger = Logger::instance();
    void run_loop();
    virtual bool stop_task_condition(TcpSocket& socket);
    void handle_client_task(TcpSocket* socket);
    virtual void handle_state_change(TcpSocket& socket) = 0;

    virtual Result<bool> handle_connected(TcpSocket& socket);
    virtual Result<bool> handle_idle(TcpSocket& socket);
    virtual Result<bool> handle_writing(TcpSocket& socket);
    virtual Result<bool> handle_reading(TcpSocket& socket);
    virtual Result<bool> handle_closing(TcpSocket& socket);

    



  public:
    TcpServer();
    virtual ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;

};