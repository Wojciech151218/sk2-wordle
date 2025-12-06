#include "server/http/http_request.h"
#include <sstream>
#include <algorithm>

HttpRequest::HttpRequest(std::string raw_request) {
    std::istringstream request_stream(raw_request);
    std::string line;
    
    // Parse request line (first line)
    if (std::getline(request_stream, line)) {
        std::istringstream request_line(line);
        std::string method_str, path_str, version_str;
        
        if (request_line >> method_str >> path_str >> version_str) {
            method = parse_method(method_str);
            path = path_str;
        }
    }
    
    // Parse headers (until empty line)
    while (std::getline(request_stream, line)) {
        // Remove carriage return if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // Empty line indicates end of headers
        if (line.empty()) {
            break;
        }
        
        // Parse header line: "Name: Value"
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string header_name = line.substr(0, colon_pos);
            std::string header_value = line.substr(colon_pos + 1);
            
            // Trim whitespace from header name and value
            header_name.erase(0, header_name.find_first_not_of(" \t"));
            header_name.erase(header_name.find_last_not_of(" \t") + 1);
            header_value.erase(0, header_value.find_first_not_of(" \t"));
            header_value.erase(header_value.find_last_not_of(" \t") + 1);
            
            headers.emplace_back(header_name, header_value);
        }
    }
    
    // Parse body (remaining content)
    std::ostringstream body_stream;
    while (std::getline(request_stream, line)) {
        if (!body_stream.str().empty()) {
            body_stream << '\n';
        }
        body_stream << line;
    }
    body = body_stream.str();
}

HttpMethod HttpRequest::get_method() const {
    return method;
}

std::string HttpRequest::get_path() const {
    return path;
}

std::unordered_map<std::string, std::string> HttpRequest::get_headers() const {
    std::unordered_map<std::string, std::string> header_map;
    for (const auto& header : headers) {
        header_map[header.get_name()] = header.get_value();
    }
    return header_map;
}

std::string HttpRequest::get_body() const {
    return body;
}

