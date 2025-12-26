#pragma once

#include "server/http/http_enums.h"
#include "server/http/http_header.h"
#include "server/utils/result.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

class HttpResponse{
  private:
    HttpVersion http_version;
    HttpStatusCode status_code;
    std::string status_message;
    std::vector<HttpHeader> headers;
    std::optional<std::string> body;

  public:
    HttpResponse(std::optional<std::string> body, HttpVersion http_version, HttpStatusCode status_code);
    static HttpResponse from_json(const Result<nlohmann::json> & json);
    HttpResponse add_header(const HttpHeader& header);
    HttpResponse add_cors_headers();
    HttpStatusCode get_status_code() const;
    std::string to_string() const;
    static HttpResponse option_response(std::vector<HttpMethod> allowed_methods);


};

