#pragma once

#include <string>
#include "server/http/http_enums.h"
#include <optional>

class Error {
  private:
    std::string message;
    std::optional<HttpStatusCode> http_status_code = std::nullopt;

  public:
    explicit Error(std::string message, std::optional<int> status_code = std::nullopt);
    const std::string& get_message() const;
    void handle_error(bool should_exit = false) const;
    std::optional<HttpStatusCode> get_http_status_code() const;
};


