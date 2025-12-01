#include "server/server_method.h"


class Router {

private:
    std::map<std::string, ServerMethod> methods;
    std::string error_message(Error error) const;
    std::string success_message(nlohmann::json success) const;
    Result<ServerMethod> get_method(std::string name) const;
  public:
    Router();
    ~Router();


    void add_method(const ServerMethod & method);
    std::string handle_request(Result<std::string> request);
};