#pragma once

#include <string>
#include <functional>
#include "server/request_body.h"
#include "utils/result.h"
#include "server/http/http_enums.h"

class ServerMethod {
  private:
    std::string path;
    HttpMethod method;
    RequestBody * method_body;
    std::function<Result<nlohmann::json>(const RequestBody&)> handler;

    

  public:
    ServerMethod(std::string path, HttpMethod method, RequestBody * method_body, std::function<Result<nlohmann::json>(const RequestBody&)> handler);
    std::string get_path() const;
    HttpMethod get_method() const;
    Result<nlohmann::json> handle_request(std::string raw_body) const;

};