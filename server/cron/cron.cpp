#include "cron.h"
#include <mutex>

static std::mutex jobs_mutex;

Cron::Cron() {
    // Members are initialized by default
}

Cron::~Cron() {
    // Stop the cron thread if running
    if (running) {
        running = false;
        if (cron_thread.joinable()) {
            cron_thread.join();
        }
    }
}

Cron::Cron(Cron&& other) noexcept 
    : jobs(std::move(other.jobs)),
      next_job_id(other.next_job_id),
      running(other.running),
      cron_thread(std::move(other.cron_thread)) {
    other.running = false;
}

Cron& Cron::operator=(Cron&& other) noexcept {
    if (this != &other) {
        // Stop current thread if running
        if (running) {
            running = false;
            if (cron_thread.joinable()) {
                cron_thread.join();
            }
        }
        
        // Move data from other
        jobs = std::move(other.jobs);
        next_job_id = other.next_job_id;
        running = other.running;
        cron_thread = std::move(other.cron_thread);
        
        other.running = false;
    }
    return *this;
}

Cron& Cron::add_job(const std::string& identifier,
                   const std::function<void()>& callback, 
                   std::chrono::milliseconds interval,
                   JobMode initial_mode) {
    std::lock_guard<std::mutex> lock(jobs_mutex);
    
    Job new_job;
    new_job.callback = callback;
    new_job.interval = interval;
    new_job.next_run = std::chrono::steady_clock::now() + interval;
    new_job.mode = initial_mode;
    
    jobs.emplace(identifier,std::move(new_job));
    
    return *this;
}

bool Cron::remove_job(const std::string& identifier) {
    std::lock_guard<std::mutex> lock(jobs_mutex);
    
    auto it = jobs.find(identifier);
    
    if (it != jobs.end()) {
        jobs.erase(it);
        return true;
    }
    
    return false;
}

void Cron::start() {
    if (running) {
        return; // Already running
    }
    
    running = true;
    
    cron_thread = std::thread([this]() {
        while (running) {
            auto now = std::chrono::steady_clock::now();
            
            {
                std::lock_guard<std::mutex> lock(jobs_mutex);
                
                // Check each job
                for (auto it = jobs.begin(); it != jobs.end(); ) {
                    auto& job = it->second;
                    if (job.mode == JobMode::OFF) {
                        ++it;
                        continue;
                    }
                    if (now >= job.next_run) {
                        // Execute the job
                        if (job.callback) {
                            job.callback();
                        }
                        
                        // Update or remove the job
                        if (job.mode == JobMode::PERIODIC) {
                            //Periodic job, update next run
                            job.next_run = now + job.interval;
                            ++it;
                        } else  {
                            //One-time job, remove it
                            job.mode = JobMode::OFF;
                            ++it;
                        }
                    } else {
                        ++it;
                    }
                }
            }
            
            // Sleep for a short period before checking again
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

std::optional<Cron::JobMode> Cron::get_job_mode(const std::string& identifier) {
    return atomic([&]() -> std::optional<Cron::JobMode> {
        auto it = jobs.find(identifier);
        if (it != jobs.end()) {
            return it->second.mode;
        }
        return std::nullopt;
    });
}

void Cron::set_job_mode(const std::string& identifier, Cron::JobMode mode) {
    atomic([&]() {
        auto it = jobs.find(identifier);
        if (it != jobs.end()) {
            it->second.mode = mode;
        }
    });
}

std::optional<std::chrono::milliseconds> Cron::get_job_interval(const std::string& identifier) {
    return atomic([&]() -> std::optional<std::chrono::milliseconds> {
        auto it = jobs.find(identifier);
        if (it != jobs.end()) {
            return it->second.interval;
        }
        return std::nullopt;
    });
}

void Cron::set_job_interval(const std::string& identifier, std::chrono::milliseconds interval) {
    atomic([&]() {
        auto it = jobs.find(identifier);
        if (it != jobs.end()) {
            it->second.interval = interval;
        }
    });
}

void Cron::set_job_settings(const std::string& identifier, JobMode mode, std::chrono::milliseconds interval) {
    set_job_mode(identifier, mode);
    set_job_interval(identifier, interval);
}