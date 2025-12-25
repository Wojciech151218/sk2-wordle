#include "cron.h"
#include <mutex>
#include <algorithm>

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

int Cron::add_job(const std::function<void()>& callback, 
                  std::chrono::milliseconds interval,
                  bool repeat) {
    std::lock_guard<std::mutex> lock(jobs_mutex);
    
    Job new_job;
    new_job.id = next_job_id++;
    new_job.callback = callback;
    new_job.interval = interval;
    new_job.next_run = std::chrono::steady_clock::now() + interval;
    new_job.repeat = repeat;
    
    jobs.push_back(new_job);
    
    return new_job.id;
}

bool Cron::remove_job(int job_id) {
    std::lock_guard<std::mutex> lock(jobs_mutex);
    
    auto it = std::find_if(jobs.begin(), jobs.end(),
                          [job_id](const Job& job) { return job.id == job_id; });
    
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
                    if (now >= it->next_run) {
                        // Execute the job
                        if (it->callback) {
                            it->callback();
                        }
                        
                        // Update or remove the job
                        if (it->repeat) {
                            it->next_run = now + it->interval;
                            ++it;
                        } else {
                            // One-time job, remove it
                            it = jobs.erase(it);
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
