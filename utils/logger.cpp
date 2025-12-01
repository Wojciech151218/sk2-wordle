#include "utils/logger.h"

#include <cerrno>
#include <cstring>
#include <utility>

namespace {
constexpr const char* COLOR_RESET = "\033[0m";
constexpr const char* COLOR_INFO = "\033[32m";
constexpr const char* COLOR_DEBUG = "\033[36m";
constexpr const char* COLOR_ERROR = "\033[31m";
}  // namespace

Logger& Logger::instance() {
    static Logger logger_instance;
    return logger_instance;
}

Logger::Logger() : Logger(std::cout, std::cerr, Options{}) {}

Logger::Logger(std::ostream& out_stream, std::ostream& err_stream, Options options)
    : out_stream(out_stream), err_stream(err_stream), options(std::move(options)) {}

void Logger::set_level_enabled(Level level, bool enabled) {
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
}

void Logger::enable(Level level) {
    set_level_enabled(level, true);
}

void Logger::disable(Level level) {
    set_level_enabled(level, false);
}

bool Logger::is_level_enabled(Level level) const {
    return level_enabled(level);
}

void Logger::set_use_colors(bool enabled) {
    options.use_colors = enabled;
}

bool Logger::uses_colors() const {
    return options.use_colors;
}

void Logger::configure(const Options& new_options) {
    options = new_options;
}

Logger::Options Logger::current_options() const {
    return options;
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
    if (!level_enabled(level)) {
        return;
    }
    write(level, message);
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
            return "INFO";
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

void Logger::write(Level level, const std::string& message) const {
    std::ostream& stream = (level == Level::Error) ? err_stream : out_stream;
    if (options.use_colors) {
        stream << level_color(level);
    }
    stream << "[" << level_name(level) << "] " << message;
    if (options.use_colors) {
        stream << COLOR_RESET;
    }
    stream << std::endl;
}

