#include "server/http/http_enums.h"

HttpMethod parse_method(const std::string& method_str) {
    if (method_str == "GET") return HttpMethod::GET;
    if (method_str == "POST") return HttpMethod::POST;
    if (method_str == "PUT") return HttpMethod::PUT;
    if (method_str == "DELETE") return HttpMethod::DELETE;
    if (method_str == "PATCH") return HttpMethod::PATCH;
    if (method_str == "OPTIONS") return HttpMethod::OPTIONS;
    if (method_str == "HEAD") return HttpMethod::HEAD;
    return HttpMethod::GET;
}


std::string get_status_message(HttpStatusCode status_code) {
    switch (status_code) {
        case HttpStatusCode::OK: return "OK";
        case HttpStatusCode::BAD_REQUEST: return "Bad Request";
        case HttpStatusCode::NOT_FOUND: return "Not Found";
        case HttpStatusCode::METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case HttpStatusCode::INTERNAL_SERVER_ERROR: return "Internal Server Error";
        default: return "OK";
    }
}