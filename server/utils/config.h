#pragma once

#include <mutex>
#include <string>
#include <optional>

class Config {
  public:
    static Config& instance();

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;

    void set_allowed_origin(const std::string& origin);
    std::string get_allowed_origin() const;

  private:
    std::optional<std::string> allowed_origin;
    mutable std::mutex mutex;

    Config();
};

