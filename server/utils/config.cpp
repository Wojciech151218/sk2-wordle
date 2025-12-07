#include "server/utils/config.h"

Config::Config() : allowed_origin("") {}

void Config::set_allowed_origin(const std::string& origin) {
    atomic([&]() {
        allowed_origin = origin;
    });
}

std::string Config::get_allowed_origin() const {
    return atomic([&]() {
        return allowed_origin.value_or("*");
    });
}

