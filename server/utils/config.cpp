#include "server/utils/config.h"

#include <mutex>

Config& Config::instance() {
    static Config config_instance;
    return config_instance;
}

Config::Config() : allowed_origin("") {}

void Config::set_allowed_origin(const std::string& origin) {
    std::lock_guard<std::mutex> lock(mutex);
    allowed_origin = origin;
}

std::string Config::get_allowed_origin() const {
    std::lock_guard<std::mutex> lock(mutex);
    return allowed_origin.value_or("*");
}

