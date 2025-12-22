#pragma once

#include "utils/result.h"
#include "nlohmann/json.hpp"
#include <memory>

class RequestBody {
  public:
    RequestBody();

    virtual ~RequestBody() = default; //dodalem bo na stacku jakis wyciek pamieci byl przez to nie wiem czy to dobrze

    virtual Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) = 0;

};