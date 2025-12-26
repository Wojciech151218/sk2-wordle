#pragma once

#include "server/utils/result.h"
#include "nlohmann/json.hpp"
#include <memory>

class RequestBody {
  public:
    RequestBody();
    

    virtual Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) = 0;

};


class EmptyRequestBody : public RequestBody {
    public:
        EmptyRequestBody() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> validate(const nlohmann::json& json) override {
            return Result<std::unique_ptr<RequestBody>>(std::make_unique<EmptyRequestBody>());
        }
};