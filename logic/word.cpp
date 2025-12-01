#include "logic/word.h"

Word::Word(std::string word) : RequestBody(), word(word) {
}

Result<std::unique_ptr<RequestBody>> Word::from_json(const nlohmann::json& json) {
    if (!json.contains("word")) {
        return Result<std::unique_ptr<RequestBody>>(Error("Word field is missing"));
    }
    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<Word>(
            json["word"].get<std::string>()
            ));
}

nlohmann::json Word::to_json() const {
    return nlohmann::json({{"word", word}});
}