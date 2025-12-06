#include "server/http/http_response.h"
#include <sstream>

HttpResponse::HttpResponse(std::string body, HttpVersion http_version, HttpStatusCode status_code)
    : body(std::move(body)), http_version(http_version), status_code(status_code) {
        status_message = status_message;
    
}

HttpResponse HttpResponse::from_json(Result<nlohmann::json> json) {
    if (json.is_err()) {
        Error error = json.unwrap_err(false);
        auto status = error.get_http_status_code();
        status = status.has_value() ? status.value() : HttpStatusCode::INTERNAL_SERVER_ERROR;
        
        return HttpResponse(
            Result<std::string>(error.get_message()),
            HttpVersion::HTTP_1_1,
            status,
            get_status_message(status)
        )
        .add_header(HttpHeader::content_type("application/json"))
        .add_header(HttpHeader::content_length(error.get_message().size()));
    }
    
    return HttpResponse(
        json.unwrap(),
        HttpVersion::HTTP_1_1,
        HttpStatusCode::OK,
        get_status_message(HttpStatusCode::OK)
    )
    .add_header(HttpHeader::content_type("application/json"))
    .add_header(HttpHeader::content_length(json.unwrap().dump()));
}

HttpResponse HttpResponse::add_header(const HttpHeader& header) {
    headers.push_back(header);
    return *this;
}
