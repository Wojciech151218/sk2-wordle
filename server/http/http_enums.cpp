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
        case HttpStatusCode::SWITCHING_PROTOCOLS: return "Switching Protocols";
        case HttpStatusCode::BAD_REQUEST: return "Bad Request";
        case HttpStatusCode::NOT_FOUND: return "Not Found";
        case HttpStatusCode::METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case HttpStatusCode::INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case HttpStatusCode::NO_CONTENT: return "No Content";
        case HttpStatusCode::FORBIDDEN: return "Forbidden";
        default: return "OK";
    }
}
HttpVersion parse_version(const std::string& version_str) {
    if (version_str == "HTTP/1.1") return HttpVersion::HTTP_1_1;
    if (version_str == "HTTP/2.0") return HttpVersion::HTTP_2_0;
    if (version_str == "HTTP/3.0") return HttpVersion::HTTP_3_0;
    return HttpVersion::HTTP_1_1;
}

std::string http_version_to_string(HttpVersion http_version) {
    switch (http_version) {
        case HttpVersion::HTTP_1_1: return "HTTP/1.1";
        case HttpVersion::HTTP_2_0: return "HTTP/2.0";
        case HttpVersion::HTTP_3_0: return "HTTP/3.0";
        default: return "HTTP/1.1";
    }
}

std::string method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE: return "DELETE";
        case HttpMethod::PATCH: return "PATCH";
        case HttpMethod::OPTIONS: return "OPTIONS";
        case HttpMethod::HEAD: return "HEAD";
        default: return "GET";
    }
}
std::string status_code_to_string(HttpStatusCode status_code) {
    switch (status_code) {
        case HttpStatusCode::OK: return "200";
        case HttpStatusCode::SWITCHING_PROTOCOLS: return "101";
        case HttpStatusCode::BAD_REQUEST: return "400";
        case HttpStatusCode::NOT_FOUND: return "404";
        case HttpStatusCode::METHOD_NOT_ALLOWED: return "405";
        case HttpStatusCode::INTERNAL_SERVER_ERROR: return "500";
        case HttpStatusCode::NO_CONTENT: return "204";
        case HttpStatusCode::FORBIDDEN: return "403";
        default: return "200";
    }
}


