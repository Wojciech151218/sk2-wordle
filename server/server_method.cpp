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



Result<nlohmann::json> ServerMethod::handle_request(nlohmann::json request) const {

    auto parse_result = method_body->from_json(request);
    if (parse_result.is_err()) {
        return Result<nlohmann::json>(parse_result.unwrap_err());
    }

    auto request_body = parse_result.unwrap();

    auto handle_result = handler(*request_body);

    return handle_result;
}