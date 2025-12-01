#pragma once

#include <string>
#include <functional>
#include "utils/result.h"
template<typename P>
class ServerMethod {
  private:
    std::string name;
    std::function<Result<P>(std::string)> parser;
    std::function<std::string(P)> handler;

    public:
        ServerMethod(std::string name, std::function<Result<P>(std::string)> parser, std::function<std::string(P)> handler);
    std::string get_name() const;
    std::string handle_request(std::string request) const;

};