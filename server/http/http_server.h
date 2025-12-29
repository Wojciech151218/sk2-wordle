#pragma once

#include "server/server/tcp_server.h"
#include "server/http/router.h"
#include "server/http/server_method.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"
#include "server/utils/result.h"

class HttpServer : public TcpServer {
  protected:
    Router router;

    Result<bool> handle_connected(TcpSocket& socket) override;
    std::string get_response_info(std::string route_name,const HttpResponse& response, const TcpSocket& socket) const;

  public:
    HttpServer();
    virtual ~HttpServer();

    void start(int port, std::string address);

    template <typename Body>
    void add_method(const ServerMethod<Body>& method) {
        router.add_method(method);
    }

    // Override virtual methods from TcpServer
    virtual void handle_state_change(TcpSocket& socket) override;
};

