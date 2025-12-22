#pragma once

#include "server/http/http_response.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"
#include <string>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>


HttpResponse handshake_response(std::string key);

Result<std::string> handshake_request(const HttpRequest& request);
std::string base64_enncode(const unsigned char* input, int length);
std::string compute_web_socket_accept(std::string client_key) ;
