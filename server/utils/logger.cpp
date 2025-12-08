#include "server/utils/logger.h"

#include "server/http/http_enums.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <utility>

namespace {
constexpr const char* COLOR_RESET = "\033[0m";
constexpr const char* COLOR_INFO = "\033[32m";
constexpr const char* COLOR_DEBUG = "\033[36m";
constexpr const char* COLOR_ERROR = "\033[31m";
}  // namespace

Logger::Logger() : Logger(std::cout, std::cerr, Options{}) {}

Logger::Logger(std::ostream& out_stream, std::ostream& err_stream, Options options)
    : out_stream(out_stream), err_stream(err_stream), options(std::move(options)) {}

void Logger::set_level_enabled(Level level, bool enabled) {
    atomic([&]() {
        switch (level) {
            case Level::Info:
                options.info_enabled = enabled;
                break;
            case Level::Debug:
                options.debug_enabled = enabled;
                break;
            case Level::Error:
                options.error_enabled = enabled;
                break;
        }
    });
}

void Logger::enable(Level level) {
    set_level_enabled(level, true);
}

void Logger::disable(Level level) {
    set_level_enabled(level, false);
}

bool Logger::is_level_enabled(Level level) const {
    return atomic([&]() {
        return level_enabled(level);
    });
}

void Logger::set_use_colors(bool enabled) {
    atomic([&]() {
        options.use_colors = enabled;
    });
}

bool Logger::uses_colors() const {
    return atomic([&]() {
        return options.use_colors;
    });
}

void Logger::set_use_timestamps(bool enabled) {
    atomic([&]() {
        options.use_timestamps = enabled;
    });
}

bool Logger::uses_timestamps() const {
    return atomic([&]() {
        return options.use_timestamps;
    });
}

void Logger::configure(const Options& new_options) {
    atomic([&]() {
        options = new_options;
    });
}

Logger::Options Logger::current_options() const {
    return atomic([&]() {
        return options;
    });
}

void Logger::info(const std::string& message) const {
    log(Level::Info, message);
}

void Logger::debug(const std::string& message) const {
    log(Level::Debug, message);
}

void Logger::error(const Error& error) const {
    const int current_errno = errno;
    const std::string errno_details = " (errno=" + std::to_string(current_errno) + ": " + std::strerror(current_errno) +
                                      ")";
    log(Level::Error, error.get_message() + errno_details);
}

void Logger::log(Level level, const std::string& message) const {
    atomic([&]() {
        if (!level_enabled(level)) {
            return;
        }
        write(level, message);
    });
}

bool Logger::level_enabled(Level level) const {
    switch (level) {
        case Level::Info:
            return options.info_enabled;
        case Level::Debug:
            return options.debug_enabled;
        case Level::Error:
            return options.error_enabled;
    }
    return false;
}

const char* Logger::level_name(Level level) const {
    switch (level) {
        case Level::Info:
            return "INFO ";
        case Level::Debug:
            return "DEBUG";
        case Level::Error:
            return "ERROR";
    }
    return "";
}

const char* Logger::level_color(Level level) const {
    switch (level) {
        case Level::Info:
            return COLOR_INFO;
        case Level::Debug:
            return COLOR_DEBUG;
        case Level::Error:
            return COLOR_ERROR;
    }
    return COLOR_RESET;
}

std::string Logger::format_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::thread_info() const {
    if (!options.debug_enabled) return "";
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    return "[THREAD " + ss.str() + "]";
}

void Logger::write(Level level, const std::string& message) const {
    std::ostream& stream = (level == Level::Error) ? err_stream : out_stream;
    if (options.use_colors) {
        stream << level_color(level);
    }
    stream << "["  << level_name(level) << " ";

    if (options.use_timestamps) {
        stream << format_timestamp();
    }
    stream << "] ";
    stream << thread_info();
    stream << " " << message;
    if (options.use_colors) {
        stream << COLOR_RESET;
    }
    stream << std::endl;
}

void Logger::request_result_info(const HttpRequest& request, const HttpResponse& response) {
    std::string info_string = 
    "[REQUEST] " + method_to_string(request.get_method()) + " " + request.get_path() + " " 
        + status_code_to_string(response.get_status_code()) + " "
        + get_status_message(response.get_status_code());
    info(info_string);
}

