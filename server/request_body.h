#pragma once

#include "utils/result.h"
#include "nlohmann/json.hpp"
#include <memory>

class RequestBody {
  public:
    RequestBody();
    

    virtual Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) = 0;

};