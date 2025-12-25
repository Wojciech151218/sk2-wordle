#pragma once
#include <vector>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>

class Cron {
private:
    struct Job {
        int id;
        std::function<void()> callback;
        std::chrono::milliseconds interval;
        std::chrono::steady_clock::time_point next_run;
        bool repeat; // true for periodic, false for one-time
    };
    
    std::vector<Job> jobs;
    int next_job_id = 0;
    bool running = false;
    std::thread cron_thread;
public:
    Cron();
    ~Cron();
    
    // Delete copy constructor and copy assignment
    Cron(const Cron&) = delete;
    Cron& operator=(const Cron&) = delete;
    
    // Allow move constructor and move assignment
    Cron(Cron&& other) noexcept;
    Cron& operator=(Cron&& other) noexcept;
    
    // Returns job ID for later removal
    int add_job(const std::function<void()>& callback, 
                std::chrono::milliseconds interval,
                bool repeat = true);
    
    bool remove_job(int job_id);
    
    void start();  //make a thread and run the cron

};