#pragma once

#include <string>

class HttpHeader{
  private:
    std::string name;
    std::string value;

  public:
    HttpHeader(std::string name, std::string value);
    std::string get_name() const;
    std::string get_value() const;

    static HttpHeader content_length(std::string content);
    static HttpHeader content_type(std::string content_type);
};

