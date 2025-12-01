#include "server/tcp_server.h"

#include <string>

TcpServer::TcpServer()
    : socket(TcpSocket()),
      thread_pool(10, [this](TcpSocket* client) { handle_client(client); }),
      router(Router()) {}

TcpServer::~TcpServer() {
    socket.disconnect();
}


void TcpServer::start(int port, std::string address) {
    Logger& logger = Logger::instance();
    logger.info("Starting TCP server on " + address + ":" + std::to_string(port));
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
    client_socket->receive()
        .chain<std::string>([&](std::string req) {
            logger.debug("Received from client: " + req);
            auto response = router.handle_request(req);
            client_socket->send(response);
            return response;
        });
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

void TcpServer::add_method(const ServerMethod & method) {
    router.add_method(method);
}