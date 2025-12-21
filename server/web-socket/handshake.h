#pragma once

#include "server/http/http_response.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"
#include <string>

HttpResponse handshake_response(const std::string& key);

Result<std::string> handshake_request(const HttpRequest& request);