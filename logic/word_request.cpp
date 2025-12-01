#include "logic/word_request.h"

WordRequest::WordRequest(std::string word) : RequestBody(), word(word) {
}

Result<std::unique_ptr<RequestBody>> WordRequest::from_json(const nlohmann::json& json) {
    if (!json.contains("word")) {
        return Result<std::unique_ptr<RequestBody>>(Error("Word field is missing"));
    }
    

    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<WordRequest>(
            json["word"].get<std::string>()
            ));
}

nlohmann::json WordRequest::validate() const {
    return nlohmann::json({"word", word});
}