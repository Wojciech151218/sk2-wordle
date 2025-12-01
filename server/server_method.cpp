#include "server/server_method.h"
#include "utils/result.h"
#include <memory>

ServerMethod::ServerMethod(std::string name, RequestBody * method_body, std::function<nlohmann::json(const RequestBody&)> handler)
    : name(name), method_body(method_body), handler(handler) {}

std::string ServerMethod::get_name() const {
    return name;
}



Result<nlohmann::json> ServerMethod::handle_request(nlohmann::json request) const {

    auto parse_result = method_body->from_json(request);
    if (parse_result.is_err()) {
        return Result<nlohmann::json>(parse_result.unwrap_err());
    }

    auto request_body = parse_result.unwrap();

    auto handle_result = handler(*request_body);

    return Result<nlohmann::json>(handle_result);
}