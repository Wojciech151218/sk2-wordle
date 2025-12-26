#pragma once

#include "server/server/tcp_server.h"
#include "server/server/router.h"
#include "server/server/server_method.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"

class HttpServer : public TcpServer {
  protected:
    Router router;

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
    virtual Result<bool> handle_idle(TcpSocket& socket) override;
    virtual Result<bool> handle_writing(TcpSocket& socket) override;
    virtual Result<bool> handle_reading(TcpSocket& socket) override;
    virtual Result<bool> handle_closing(TcpSocket& socket) override;
};

