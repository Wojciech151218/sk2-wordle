#pragma once

#include <string>

class Error {
  private:
    std::string message;

  public:
    explicit Error(std::string message);
    const std::string& get_message() const;
    void handle_error(bool should_exit = false) const;
};


