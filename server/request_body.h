#pragma once

#include "utils/result.h"
#include "nlohmann/json.hpp"
#include <memory>

class RequestBody {
  public:
    RequestBody();
    

    virtual Result<std::unique_ptr<RequestBody>> from_json(const nlohmann::json& json) = 0;
    virtual nlohmann::json to_json() const = 0;

};