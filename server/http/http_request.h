#pragma once

#include "server/http/http_enums.h"
#include "server/http/http_header.h"
#include <string>
#include <vector>
#include <unordered_map>

class HttpRequest{
  private:
    HttpMethod method;
    std::string path;
    std::vector<HttpHeader> headers;
    std::string body;
    HttpVersion version;

  public:
    HttpRequest(std::string raw_request);

    HttpMethod get_method() const;
    std::string get_path() const;
    std::unordered_map<std::string, std::string> get_headers() const;
    std::string get_body() const;
    std::string to_string() const;
};

