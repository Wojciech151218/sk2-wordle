#pragma once

#include <string>
#include "server/http/http_enums.h"
#include <optional>

class Error {
  private:
    std::string message;
    HttpStatusCode http_status_code = HttpStatusCode::INTERNAL_SERVER_ERROR;

  public:
    explicit Error(std::string message, HttpStatusCode http_status_code = HttpStatusCode::INTERNAL_SERVER_ERROR);
    const std::string& get_message() const;
    void handle_error(bool should_exit = false) const;
    HttpStatusCode get_http_status_code() const;
};


