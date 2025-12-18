#include "server/router.h"
#include "server/http/http_request.h"
#include "utils/error.h"

Router::Router() {}

Router::~Router() {}

Result<const ServerMethodBase*> Router::get_method(const HttpRequest& http_request) const {
    const auto path_it = methods.find(http_request.get_path());
    if (path_it == methods.end()) {
        return Result<const ServerMethodBase*>(
            Error("Path not found", HttpStatusCode::NOT_FOUND));
    }

    const auto method_it = path_it->second.find(http_request.get_method());
    if (method_it == path_it->second.end()) {
        return Result<const ServerMethodBase*>(
            Error("Method not allowed for this path", HttpStatusCode::METHOD_NOT_ALLOWED));
    }
    return Result<const ServerMethodBase*>(method_it->second.get());
}

std::vector<HttpMethod> Router::get_allowed_methods(const std::string& path) const {
    const auto path_it = methods.find(path);
    if (path_it == methods.end()) {
        return std::vector<HttpMethod>();
    }
    std::vector<HttpMethod> allowed_methods;
    for (const auto& method : path_it->second) {
        allowed_methods.push_back(method.first);
    }
    return allowed_methods;
}

HttpResponse Router::option_response(const HttpRequest& request) {
    const std::string path = request.get_path();
    const auto path_it = methods.find(path);
    if (path_it == methods.end()) {
        return HttpResponse::option_response(std::vector<HttpMethod>());
    }
    std::vector<HttpMethod> allowed_methods;
    for (const auto& method : path_it->second) {
        allowed_methods.push_back(method.first);
    }
    return HttpResponse::option_response(allowed_methods);
}

HttpResponse Router::handle_request(const HttpRequest& http_request) {
    if (http_request.get_method() == HttpMethod::OPTIONS) {
        return option_response(http_request);
    }

    auto method_result = get_method(http_request);
    if (method_result.is_err()) {
        return HttpResponse::from_json(
            Result<nlohmann::json>(method_result.unwrap_err()));
    }

    const ServerMethodBase* method = method_result.unwrap();
    auto response = method->handle_request(http_request.get_body());
    return HttpResponse::from_json(response);
}

void Router::log_methods() {
    Logger& logger = Logger::instance();
    for (const auto& [path, method_map] : methods) {
        for (const auto& [method, method_ptr] : method_map) {
            logger.info("registered route: " + path + " " + method_to_string(method));
        }
    }
}