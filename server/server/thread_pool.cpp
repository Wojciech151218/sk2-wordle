#include "server/server/thread_pool.h"
#include "server/server/tcp_socket.h"
#include <stdexcept>

ThreadPool::ThreadPool(size_t n,std::function<void(TcpSocket&)> handle_job_callback) : 
    handle_job_callback(handle_job_callback), stop_flag(false) {
        workers.reserve(n);
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

void ThreadPool::enqueue(TcpSocket& socket) {
    {
        std::unique_lock<std::mutex> lock(mtx);
        TcpSocket* socket_ptr = &socket;
        auto& state = socket_states[socket_ptr];
        state.pending++;
        if (!state.active && !state.queued) {
            job_queue.push(&socket);
            state.queued = true;
        }
    }
    cv.notify_one();
}

void ThreadPool::dequeue(TcpSocket& socket) {
    std::unique_lock<std::mutex> lock(mtx);
    TcpSocket* target = &socket;

    if (!job_queue.empty()) {
        std::queue<TcpSocket*> new_queue;
        while (!job_queue.empty()) {
            TcpSocket* candidate = job_queue.front();
            job_queue.pop();
            if (candidate == target) {
                continue;
            }
            new_queue.push(candidate);
        }
        job_queue.swap(new_queue);
    }

    socket_states.erase(target);
}

void ThreadPool::worker_loop() {
    while (true) {
        TcpSocket* socket_ptr = nullptr;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return stop_flag || !job_queue.empty(); });
            if (stop_flag && job_queue.empty()) return;
            socket_ptr = job_queue.front();
            job_queue.pop();
            auto& state = socket_states[socket_ptr];
            state.queued = false;
            state.active = true;
            if (state.pending > 0) {
                state.pending--;
            }
        }
        if (socket_ptr == nullptr) {
            throw std::runtime_error("Socket pointer is null");
        }

        bool rethrow = false;
        try {
            handle_job_callback(*socket_ptr);
        } catch (...) {
            rethrow = true;
        }

        {
            std::unique_lock<std::mutex> lock(mtx);
            auto it = socket_states.find(socket_ptr);
            if (it != socket_states.end()) {
                auto& state = it->second;
                state.active = false;
                if (state.pending > 0) {
                    job_queue.push(socket_ptr);
                    state.queued = true;
                    cv.notify_one();
                } else {
                    socket_states.erase(it);
                }
            }
        }

        if (rethrow) throw;
    }
}
