#include "server/http/http_header.h"

HttpHeader::HttpHeader(std::string name, std::string value) 
    : name(std::move(name)), value(std::move(value)) {}

std::string HttpHeader::get_name() const {
    return name;
}

std::string HttpHeader::get_value() const {
    return value;
}

HttpHeader HttpHeader::content_length(std::string content) {
    return HttpHeader("Content-Length", std::to_string(content.size()));
}

HttpHeader HttpHeader::content_type(std::string content_type) {
    return HttpHeader("Content-Type", content_type);
}
