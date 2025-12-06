#include "utils/error.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <optional>

Error::Error(std::string message, std::optional<int> status_code = std::nullopt) 
    : message(std::move(message)), status_code(std::move(status_code)) {}

const std::string& Error::get_message() const {
    return message;
}

void Error::handle_error(bool should_exit) const {
    std::cout << message << " " << errno << " " << strerror(errno) << std::endl;
    if (should_exit) {
        std::exit(EXIT_FAILURE);
    }
    throw std::runtime_error(message);
}

std::optional<HttpStatusCode> Error::get_http_status_code() const {
    return http_status_code;
}
