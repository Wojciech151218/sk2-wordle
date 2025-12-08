#include "logic/word_request.h"

WordRequest::WordRequest(std::string word) : RequestBody(), word(word) {
}

Result<std::unique_ptr<RequestBody>> WordRequest::validate(const nlohmann::json& json) {
    if (!json.contains("word")) {
        return Error("Word field is missing", HttpStatusCode::BAD_REQUEST);
    }
    

    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<WordRequest>(json["word"].get<std::string>())
    );
}
