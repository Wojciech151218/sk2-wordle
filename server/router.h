#include "server/server_method.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"

class Router {

private:
    std::map<std::string, std::map<HttpMethod ,ServerMethod>> methods;
    Result<ServerMethod> get_method(const HttpRequest& http_request) const;
  public:
    Router();
    ~Router();


    void add_method(const ServerMethod & method);
    HttpResponse handle_request(Result<HttpRequest> request);
    HttpResponse option_response(const HttpRequest& request);
    std::vector<HttpMethod> get_allowed_methods(const std::string& path) const;
};