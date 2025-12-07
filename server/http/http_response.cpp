#include "server/http/http_response.h"
#include "server/http/http_enums.h"
#include "server/utils/config.h"
#include <sstream>

HttpResponse::HttpResponse(std::optional<std::string> body, HttpVersion http_version, HttpStatusCode status_code)
    : body(std::move(body)), http_version(http_version), status_code(status_code) {
        status_message = get_status_message(status_code);
        
    
}

HttpResponse HttpResponse::from_json(Result<nlohmann::json> json) {
    if (json.is_err()) {
        Error error = json.unwrap_err(false);
        auto status = error.get_http_status_code();
        std::string error_message = nlohmann::json({{"message", error.get_message()}}).dump();

        return HttpResponse(
            error_message,
            HttpVersion::HTTP_1_1,
            status
        )
        .add_header(HttpHeader::content_type("application/json"))
        .add_header(HttpHeader::content_length(error_message))
        .add_cors_headers();
    }
    
    std::string response_body = json.unwrap().dump();
    return HttpResponse(
        response_body,
        HttpVersion::HTTP_1_1,
        HttpStatusCode::OK
    )
    .add_header(HttpHeader::content_type("application/json"))
    .add_header(HttpHeader::content_length(response_body))
    .add_cors_headers();
}

HttpResponse HttpResponse::add_header(const HttpHeader& header) {
    headers.push_back(header);
    return *this;
}

std::string HttpResponse::to_string() const {
    std::stringstream response_stream;
    response_stream 
    << http_version_to_string(http_version) 
    << " " << status_code_to_string(status_code) 
    << " " << get_status_message(status_code) << "\r\n";
    
    for (const auto& header : headers) {
        response_stream << header.get_name() << ": " << header.get_value() << "\r\n";
    }
    response_stream << "\r\n" << body.value_or("");
    return response_stream.str();
}

HttpResponse HttpResponse::add_cors_headers() {
    
    Config& config = Config::instance();
    std::string allowed_origin = config.get_allowed_origin();

    add_header(HttpHeader("Access-Control-Allow-Origin", allowed_origin));
    add_header(HttpHeader("Access-Control-Allow-Headers", "Content-Type"));
    add_header(HttpHeader("Access-Control-Allow-Credentials", "true"));
    add_header(HttpHeader("Access-Control-Max-Age", "86400"));
    add_header(HttpHeader("Access-Control-Expose-Headers", "Content-Type"));
    add_header(HttpHeader("Connection", "keep-alive"));
    add_header(HttpHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE, PATCH"));

    return *this;
}

HttpResponse HttpResponse::option_response(std::vector<HttpMethod> allowed_methods) {
    std::string allowed_methods_string = "";
    for (const auto& method : allowed_methods) {
        allowed_methods_string += method_to_string(method) + ", ";
    }
    allowed_methods_string = allowed_methods_string.substr(0, allowed_methods_string.size() - 2);

    return HttpResponse(
        std::nullopt,
        HttpVersion::HTTP_1_1,
        HttpStatusCode::NO_CONTENT
    )
    .add_cors_headers()
    .add_header(HttpHeader("Allow", allowed_methods_string));
}