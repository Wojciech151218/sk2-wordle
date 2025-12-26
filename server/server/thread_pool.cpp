#include "server/server/thread_pool.h"

ThreadPool::ThreadPool(size_t n,std::function<void(TcpSocket*)> handle_job_callback) : 
    handle_job_callback(handle_job_callback), stop_flag(false), workers(n) {
        for (size_t i = 0; i < n; ++i) {
            workers.emplace_back(&ThreadPool::worker_loop, this);
        }

}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop_flag = true;
    }
    cv.notify_all();
    for (auto& t : workers) t.join();
}

void ThreadPool::enqueue(TcpSocket* socket) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        job_queue.push(socket);
    }
    cv.notify_one();
}

void ThreadPool::worker_loop() {
    while (true) {
        TcpSocket* socket;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return stop_flag || !job_queue.empty(); });
            if (stop_flag && job_queue.empty()) return;
            socket = job_queue.front();
            job_queue.pop();
        }

        handle_job_callback(socket);
    }
}
