#include "server/server_method.h"
#include "utils/result.h"
#include <memory>

ServerMethod::ServerMethod(
    std::string path,
    HttpMethod method, 
    RequestBody * method_body, 
    std::function<Result<nlohmann::json>(const RequestBody&)> handler
)
    : path(path), method(method), method_body(method_body), handler(std::move(handler)) {}

std::string ServerMethod::get_path() const {
    return path;
}
HttpMethod ServerMethod::get_method() const {
    return method;
}



Result<nlohmann::json> ServerMethod::handle_request(std::string raw_body) const {

    nlohmann::json json_body;
    try {
        json_body = nlohmann::json::parse(raw_body);
    } catch (const nlohmann::json::parse_error& e) {
        return Result<nlohmann::json>(Error(
            "Invalid JSON format: " + std::string(e.what()), 
            HttpStatusCode::BAD_REQUEST)
        );
    }

    auto parse_result = method_body->validate(json_body);
    if (parse_result.is_err()) {
        return Result<nlohmann::json>(parse_result.unwrap_err());
    }

    auto request_body = parse_result.unwrap();

    auto handle_result = handler(*request_body);

    return handle_result;
}