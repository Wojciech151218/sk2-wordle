#include "server/router.h"
#include "utils/error.h"

Router::Router() {}

Router::~Router() {}

void Router::add_method(const ServerMethod & method) {
    methods.insert({method.get_name(), method});
}

std::string Router::error_message(Error error) const {
    Logger& logger = Logger::instance();
    logger.error(error);
    return nlohmann::json({{"status", "error"}, {"message", error.get_message()}}).dump();
}

std::string Router::success_message(nlohmann::json success) const {
    Logger& logger = Logger::instance();
    nlohmann::json response = {{"status", "ok"}};
    response.update(success);
    logger.debug("response: " + response.dump());
    return response.dump();
}

Result<ServerMethod> Router::get_method(std::string name) const {
    if (methods.find(name) == methods.end()) {
        return Result<ServerMethod>(Error("Method not found"));
    }
    return Result<ServerMethod>(methods.find(name)->second);
}


std::string Router::handle_request(Result<std::string> request) {

    if (request.is_err()) {
        return error_message(request.unwrap_err());
    }
    nlohmann::json request_json;
    try {
        request_json = nlohmann::json::parse(request.unwrap());
    } catch (const nlohmann::json::parse_error& e) {
        return error_message(Error("Invalid JSON format"));
    }

    auto method_name = request_json["method"].get<std::string>();
    if (method_name.empty()) {
        return error_message(Error("Method name field is empty"));
    }

    auto method_result = get_method(method_name);
    if (method_result.is_err()) {
        return error_message(method_result.unwrap_err());
    }

    auto method = method_result.unwrap();   
    auto response = method.handle_request(request_json);

    if (response.is_err()) {
        return error_message(response.unwrap_err());
    }

    return success_message(response.unwrap());
}