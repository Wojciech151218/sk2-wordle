#pragma once
#include <chrono>
#include <functional>
#include <thread>
#include <string>
#include <unordered_map>
#include "server/utils/global_state.h"
#include <optional>

class Cron : public GlobalState<Cron> {
public:
    enum class JobMode{
        ONCE,
        PERIODIC,
        OFF
    };
    
private:
    struct Job {
        std::function<void()> callback;
        std::chrono::milliseconds interval;
        std::chrono::steady_clock::time_point next_run;
        JobMode mode;
    };
    
    std::unordered_map<std::string, Job> jobs;
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
    
    // Returns reference to Cron for method chaining
    Cron& add_job(const std::string& identifier,
                 const std::function<void()>& callback, 
                 std::chrono::milliseconds interval,
                 JobMode initial_mode = JobMode::PERIODIC
                 );
    
    bool remove_job(const std::string& identifier);

    std::optional<JobMode> get_job_mode(const std::string& identifier);
    void set_job_mode(const std::string& identifier, JobMode mode);

    std::optional<std::chrono::milliseconds> get_job_interval(const std::string& identifier);
    void set_job_interval(const std::string& identifier, std::chrono::milliseconds interval);

    void reset_job_next_run(const std::string& identifier);
    void set_job_settings(const std::string& identifier, JobMode mode, std::chrono::milliseconds interval);

    void start();  //make a thread and run the cron

};