#include "server/server_method.h"


template<typename P>
ServerMethod<P>::ServerMethod(std::string name, std::function<Result<P>(std::string)> parser, std::function<std::string(P)> handler)
    : name(name), parser(parser), handler(handler) {}

template<typename P>
std::string ServerMethod<P>::get_name() const {
    return name;
}

template<typename P>
std::string ServerMethod<P>::handle_request(std::string request) const {

    auto parse_result = parser(request);
    if (parse_result.is_err()) {
        return parse_result.unwrap_err().get_message();
    }

    return handler(parse_result.unwrap());
}