#pragma once

#include "server/tcp_socket.h"
#include "server/thread_pool.h"
#include "server/router.h"

#include <chrono>
#include <thread>

class TcpServer {
  protected:
    TcpSocket socket;
    ThreadPool thread_pool;
    Router router;
    std::chrono::milliseconds client_timeout;
    std::thread server_thread;
    virtual void run_loop();




  public:
    TcpServer();
    virtual ~TcpServer();

    void start(int port, std::string address);
    void stop();
    void run();
    virtual void handle_client(TcpSocket* socket);
    template <typename Body>
    void add_method(const ServerMethod<Body>& method) {
        router.add_method(method);
    }
    void set_client_timeout(std::chrono::milliseconds timeout);
    std::chrono::milliseconds get_client_timeout() const;

};