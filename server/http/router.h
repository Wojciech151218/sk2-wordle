#pragma once

#include <map>
#include <memory>
#include <vector>

#include "server/http/http_request.h"
#include "server/http/http_response.h"
#include "server/http/server_method.h"

class Router {
  private:
    std::map<std::string, std::map<HttpMethod, std::unique_ptr<ServerMethodBase>>> methods;
    Result<const ServerMethodBase*> get_method(const HttpRequest& http_request) const;

  public:
    Router();
    ~Router();

    template <typename Body>
    void add_method(const ServerMethod<Body>& method) {
        methods[method.get_path()].insert(
            {method.get_method(), std::make_unique<ServerMethod<Body>>(method)});
    };
    void log_methods();

    HttpResponse handle_request(const HttpRequest& request);
    HttpResponse option_response(const HttpRequest& request);
    std::vector<HttpMethod> get_allowed_methods(const std::string& path) const;
};