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

    std::string get_response_info(const HttpRequest& http_request,const HttpResponse& response, const TcpSocket& socket) const;
    void handle_message(TcpSocket& socket, std::string message) override;
    void on_client_connected(TcpSocket& client_socket) override;

  public:
    HttpServer();
    virtual ~HttpServer();

    void start(int port, std::string address);

    template <typename Body>
    void add_method(const ServerMethod<Body>& method) {
        router.add_method(method);
    }
    // Override virtual methods from TcpServer
  };

