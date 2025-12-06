#pragma once

#include <string>

enum class HttpMethod{
  GET,
  POST,
  PUT,
  DELETE,
  PATCH,
  OPTIONS,
  HEAD,
};

enum class HttpStatusCode{
  OK = 200, 
  BAD_REQUEST = 400,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  INTERNAL_SERVER_ERROR = 500,
};

enum class HttpVersion{
  HTTP_1_1,
  HTTP_2_0,
  HTTP_3_0,
};

// Parse HTTP method from string
HttpMethod parse_method(const std::string& method_str);

