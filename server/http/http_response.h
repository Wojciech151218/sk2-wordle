#pragma once

#include "server/http/http_enums.h"
#include "server/http/http_header.h"
#include "utils/result.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class HttpResponse{
  private:
    HttpVersion http_version;
    HttpStatusCode status_code;
    std::string status_message;
    std::vector<HttpHeader> headers;
    std::string body;

  public:
    HttpResponse(std::string body, HttpVersion http_version, HttpStatusCode status_code);
    static HttpResponse from_json(Result<nlohmann::json> json);
    HttpResponse add_header(const HttpHeader& header);
    std::string to_string() const;


};

