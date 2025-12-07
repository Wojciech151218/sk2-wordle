#pragma once

#include <mutex>

template<typename Derived>
class GlobalState {
  public:
    static Derived& instance() {
        static Derived instance;
        return instance;
    }

    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;
    GlobalState(GlobalState&&) = delete;
    GlobalState& operator=(GlobalState&&) = delete;

  protected:
    GlobalState() = default;
    ~GlobalState() = default;

    // Executes a function atomically (with mutex lock)
    template<typename Func>
    auto atomic(Func&& func) const -> decltype(func()) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        return func();
    }

    // Overload for non-const methods
    template<typename Func>
    auto atomic(Func&& func) -> decltype(func()) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        return func();
    }

    // Mutable mutex to allow locking in const methods
    mutable std::mutex mutex;
};

