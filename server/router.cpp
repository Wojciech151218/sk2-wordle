#include "server/router.h"
#include "utils/error.h"
#include "server/http/http_request.h"


Router::Router() {}

Router::~Router() {}

void Router::add_method(const ServerMethod & method) {
    methods[method.get_path()].insert({method.get_method(), method});
}


Result<ServerMethod> Router::get_method(const HttpRequest& http_request) const {
    const auto path_it = methods.find(http_request.get_path());
    if (path_it == methods.end()) {
        return Result<ServerMethod>(Error("Path not found", HttpStatusCode::NOT_FOUND));
    }
    
    const auto method_it = path_it->second.find(http_request.get_method());
    if (method_it == path_it->second.end()) {
        return Result<ServerMethod>(Error("Method not allowed for this path", HttpStatusCode::METHOD_NOT_ALLOWED));
    }   
    return Result<ServerMethod>(method_it->second);
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

HttpResponse Router::handle_request(Result<HttpRequest> request) {

    if (request.is_err()) {
        return HttpResponse::from_json(
            Result<nlohmann::json>(request.unwrap_err())
        );
    }

    HttpRequest http_request = request.unwrap();

    if (http_request.get_method() == HttpMethod::OPTIONS) {
        return option_response(http_request);
    }

    nlohmann::json request_body;
    try {
        request_body = nlohmann::json::parse(http_request.get_body());
    } catch (const nlohmann::json::parse_error& e) {
        return HttpResponse::from_json(
            Result<nlohmann::json>(Error("Invalid JSON format", HttpStatusCode::BAD_REQUEST))
        );
    }


    auto method_result = get_method(http_request);
    if (method_result.is_err()) {
        return HttpResponse::from_json(
            Result<nlohmann::json>(method_result.unwrap_err())
        );
    }

    auto method = method_result.unwrap();   
    auto response = method.handle_request(request_body);
    return HttpResponse::from_json(response);
}