#include "server/utils/config.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

Config::Config() : allowed_origin("") {}

void Config::set_allowed_origin(const std::string& origin) {
    allowed_origin = origin;
}

std::string Config::get_allowed_origin() const {
    return allowed_origin.value_or("*");
}

void Config::load_config() {
    try {
        std::ifstream config_file("conf.json");
        if (!config_file.is_open()) {
            Logger::instance().warn("Could not open conf.json, using default config");
            atomic([&]() {
                config = {
                    {"allowed_origin", "http://localhost:5173"}
                };
            });
            return;
        }

        json j;
        config_file >> j;
        config_file.close();

        // Load all config values from JSON
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                config[key] = value.get<std::string>();
            }
        }

        // If allowed_origin exists, call the setter
        if (j.contains("allowed_origin") && j["allowed_origin"].is_string()) {
            set_allowed_origin(j["allowed_origin"].get<std::string>());
        }

    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        config = {
            {"allowed_origin", "*"}
        };
    }
}

std::optional<std::string> Config::get_config(const std::string& key) const {
    Logger& logger = Logger::instance();
    try {
        return config.at(key);
    } catch (const std::out_of_range& e) {
        Logger::instance().warn("Key " + key + " not found in config");
        return std::nullopt;
    }
}

void Config::set_logger_options() {
    Logger& logger = Logger::instance();
    Logger::Options options{};
    options.info_enabled = get_config("info").value_or("true") == "true";
    options.debug_enabled = get_config("debug").value_or("false") == "true";
    options.error_enabled = get_config("error").value_or("true") == "true";
    options.warn_enabled = get_config("warn").value_or("true") == "true";
    options.use_colors = get_config("use_colors").value_or("true") == "true";
    logger.configure(options);
}