#include "logic/endpoints/request_bodies.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {

Result<std::time_t> parse_timestamp(const nlohmann::json& timestamp_json) {
    if (!timestamp_json.is_string()) {
        return Error(
            "Timestamp must be a string in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)",
            HttpStatusCode::BAD_REQUEST
        );
    }

    const auto& timestamp_str = timestamp_json.get_ref<const std::string&>();
    std::tm tm{};
    std::istringstream ss(timestamp_str);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    if (ss.fail()) {
        return Error(
            "Timestamp must follow ISO 8601 format YYYY-MM-DDTHH:MM:SSZ",
            HttpStatusCode::BAD_REQUEST
        );
    }

    const std::time_t parsed_timestamp = timegm(&tm);
    if (parsed_timestamp == static_cast<std::time_t>(-1)) {
        return Error("timestamp value is invalid", HttpStatusCode::BAD_REQUEST);
    }

    return Result<std::time_t>(parsed_timestamp);
}

}  // namespace


Result<std::unique_ptr<RequestBody>> JoinRequest::validate(const nlohmann::json& json) {
    if (!json.contains("player_name")) {
        return Error("player_name field is missing", HttpStatusCode::BAD_REQUEST);
    }
    
    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<JoinRequest>(json["player_name"].get<std::string>())
    );
}

Result<std::unique_ptr<RequestBody>> StateRequest::validate(const nlohmann::json& json) {
    if (!json.contains("player_name")) {
        return Error("player_name field is missing", HttpStatusCode::BAD_REQUEST);
    }
    if (!json.contains("timestamp")) {
        return Error("timestamp field is missing", HttpStatusCode::BAD_REQUEST);
    }
    
    auto parsed_timestamp = parse_timestamp(json["timestamp"]);
    if (parsed_timestamp.is_err()) {
        return parsed_timestamp.unwrap_err(false);
    }

    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<StateRequest>(
            json["player_name"].get<std::string>(),
            parsed_timestamp.unwrap()
        )
    );
}

Result<std::unique_ptr<RequestBody>> GuessRequest::validate(const nlohmann::json& json) {
    if (!json.contains("guess")) {
        return Error("guess field is missing", HttpStatusCode::BAD_REQUEST);
    }
    
    auto parsed_timestamp = parse_timestamp(json["timestamp"]);
    if (parsed_timestamp.is_err()) {
        return parsed_timestamp.unwrap_err(false);
    }

    return Result<std::unique_ptr<RequestBody>>(
        std::make_unique<GuessRequest>(
            json["player_name"].get<std::string>(),
            parsed_timestamp.unwrap(), 
            json["guess"].get<std::string>())
    );
}

