#pragma once

#include "server/request_body.h"
#include <string>

class Word : public RequestBody {
    public:
        Word(std::string word);
        Word() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> from_json(const nlohmann::json& json) override;
        nlohmann::json to_json() const override;
        std::string word;
    private:
};