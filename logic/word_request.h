#pragma once

#include "server/request_body.h"
#include <string>

class WordRequest : public RequestBody {

    public:
     
        WordRequest(std::string word);
        WordRequest() : RequestBody() {};
        Result<std::unique_ptr<RequestBody>> from_json(const nlohmann::json& json) override;
        std::string word;
    private:
};