#include "utils/error.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <utility>

Error::Error(std::string message) : message(std::move(message)) {}

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


