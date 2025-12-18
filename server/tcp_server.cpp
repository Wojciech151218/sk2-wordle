#include "server/tcp_server.h"
#include "server/utils/logger.h"

#include <string>
#include <chrono>

TcpServer::TcpServer()
    : socket(TcpSocket()),
      thread_pool(10, [this](TcpSocket* client) { handle_client(client); }),
      router(Router()),
      client_timeout(std::chrono::seconds(30)) {}

TcpServer::~TcpServer() {
    socket.disconnect();
}


void TcpServer::start(int port, std::string address) {
    Logger& logger = Logger::instance();
    logger.info("Starting HTTP/TCP server on " + address + ":" + std::to_string(port));
    router.log_methods();
    socket.listen(address, port)
        .finally<void*>([&]() {
            logger.debug("Listening for incoming connections...");
            return nullptr;
        });
}

void TcpServer::stop() {
    Logger::instance().info("Stopping TCP server");
    socket.disconnect();
}

void TcpServer::handle_client(TcpSocket* client_socket) {
    Logger& logger = Logger::instance();

    while (true) {
        auto request = client_socket->receive(client_timeout);
        if (request.is_err()) {
            client_socket->disconnect();
            break;
        }
        HttpRequest http_request(request.unwrap());

        logger.debug("Received from client: " + http_request.to_string());
        auto response = router.handle_request(http_request);
        logger.request_result_info(http_request, response, client_socket->get_host(), client_socket->get_port());

        logger.debug("Sending response to client: " + response.to_string());

        auto send_result = client_socket->send(response.to_string());
        if (send_result.is_err()) {
            logger.error(send_result.unwrap_err());
            client_socket->disconnect();
            break;
        }
    }

    delete client_socket;
}



void TcpServer::run() {
    Logger& logger = Logger::instance();

    while (true) {
        auto accept_result = socket.accept();
        if (accept_result.is_err()) {
            logger.error(accept_result.unwrap_err());
            break;
        }
        TcpSocket * client_socket = new TcpSocket(accept_result.unwrap());
        thread_pool.enqueue(client_socket);
    }
    
}

void TcpServer::set_client_timeout(std::chrono::milliseconds timeout) {
    client_timeout = timeout;
}

std::chrono::milliseconds TcpServer::get_client_timeout() const {
    return client_timeout;
}