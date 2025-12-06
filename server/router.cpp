#include "server/router.h"
#include "utils/error.h"
#include "server/http/http_request.h"


Router::Router() {}

Router::~Router() {}

void Router::add_method(const ServerMethod & method) {
    methods[method.get_path()].insert({method.get_method(), method});
}


Result<ServerMethod> Router::get_method(HttpRequest http_request) const {
    auto path_it = methods.find(http_request.get_path());
    if (path_it == methods.end()) {
        return Result<ServerMethod>(Error("Path not found", HttpStatusCode::NOT_FOUND));
    }
    
    auto method = path_it->second.find(http_request.get_method());
    if (method == path_it->second.end()) {
        return Result<ServerMethod>(Error("Method not found", HttpStatusCode::NOT_FOUND));
    }   
    return Result<ServerMethod>(method->second);
}


HttpResponse Router::handle_request(Result<HttpRequest> request) {

    if (request.is_err()) {
        return HttpResponse::from_json(
            Result<nlohmann::json>(request.unwrap_err())
        );
    }

    HttpRequest http_request = request.unwrap();

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