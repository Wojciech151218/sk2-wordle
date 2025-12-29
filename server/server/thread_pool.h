#pragma once

#include "tcp_socket.h"
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <memory>

class ThreadPool {
public:
    ThreadPool(size_t n,std::function<void(TcpSocket&)> handle_job_callback);
    ~ThreadPool();
    void enqueue(TcpSocket& socket);

private:
    std::function<void(TcpSocket&)> handle_job_callback;
    std::vector<std::thread> workers;
    std::queue<std::reference_wrapper<TcpSocket>> job_queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop_flag;

    void worker_loop();
};