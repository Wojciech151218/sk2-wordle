#pragma once

#include "server/utils/error.h"
#include "server/utils/global_state.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class Logger : public GlobalState<Logger> {
  public:
    enum class Level { Info, Debug, Error };

    struct Options {
        bool info_enabled = true;
        bool debug_enabled = true;
        bool error_enabled = true;
        bool use_colors = true;
        bool use_timestamps = true;
    };

    void set_level_enabled(Level level, bool enabled);
    void enable(Level level);
    void disable(Level level);
    bool is_level_enabled(Level level) const;

    void set_use_colors(bool enabled);
    bool uses_colors() const;

    void set_use_timestamps(bool enabled);
    bool uses_timestamps() const;

    void configure(const Options& options);
    Options current_options() const;

    void info(const std::string& message) const;
    void debug(const std::string& message) const;
    void error(const Error& error) const;
    void log(Level level, const std::string& message) const;

  private:
    std::ostream& out_stream;
    std::ostream& err_stream;
    Options options;

    Logger();
    Logger(std::ostream& out_stream, std::ostream& err_stream, Options options);
    friend class GlobalState<Logger>;

    bool level_enabled(Level level) const;
    const char* level_name(Level level) const;
    const char* level_color(Level level) const;
    std::string format_timestamp() const;
    void write(Level level, const std::string& message) const;
};

