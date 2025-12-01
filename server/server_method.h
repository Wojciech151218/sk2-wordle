#pragma once

#include <string>
#include <functional>
#include "server/request_body.h"
#include "utils/result.h"

class ServerMethod {
  private:
    std::string name;
    RequestBody * method_body;
    std::function<nlohmann::json(const RequestBody&)> handler;

    

  public:
    ServerMethod(std::string name, RequestBody * method_body, std::function<nlohmann::json(const RequestBody&)> handler);
    std::string get_name() const;
    Result<nlohmann::json> handle_request(nlohmann::json request) const;

};