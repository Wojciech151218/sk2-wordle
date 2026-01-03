#pragma once

#include "tcp_socket.h"
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <unordered_map>

class ThreadPool {
public:
    ThreadPool(size_t n,std::function<void(TcpSocket&)> handle_job_callback);
    ~ThreadPool();
    void enqueue(TcpSocket& socket);
    void dequeue(TcpSocket& socket);

private:
    struct SocketState {
        std::size_t pending = 0;
        bool active = false;
        bool queued = false;
    };

    std::function<void(TcpSocket&)> handle_job_callback;
    std::vector<std::thread> workers;
    std::queue<TcpSocket*> job_queue;
    std::unordered_map<TcpSocket*, SocketState> socket_states;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_flag;

    void worker_loop();
};