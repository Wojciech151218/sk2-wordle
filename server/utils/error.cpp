#include "server/utils/error.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

Error::Error(std::string message, HttpStatusCode http_status_code) 
    : message(std::move(message)), http_status_code(http_status_code) {
        errno_value = errno;
    }

std::string Error::get_message(bool include_errno) const {
    if (include_errno && errno != 0) {
        return message + " (errno=" + std::to_string(errno_value) + ": " + std::strerror(errno_value) + ")";
    }
    return message;
}



void Error::handle_error(bool should_exit) const {
    std::cout << message << " " << errno_value << " " << strerror(errno_value) << std::endl;
    if (should_exit) {
        std::exit(EXIT_FAILURE);
    }
    throw std::runtime_error(message);
}

HttpStatusCode Error::get_http_status_code() const {
    return http_status_code;
}
