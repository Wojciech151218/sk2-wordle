#include "server/http/http_server.h"
#include "server/utils/logger.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"
#include <string>

HttpServer::HttpServer() : TcpServer() {}

HttpServer::~HttpServer() {}

void HttpServer::start(int port, std::string address) {
    Logger& logger = Logger::instance();
    logger.info("Starting HTTP server on " + address + ":" + std::to_string(port));
    router.log_methods();
    TcpServer::start(port, address);
}

Result<bool> HttpServer::handle_idle(TcpSocket& socket) {
    return handle_reading(socket);
}

Result<bool> HttpServer::handle_writing(TcpSocket& socket) {
    return socket.send();
}

Result<bool> HttpServer::handle_reading(TcpSocket& socket) {
    return socket.receive([](std::string data) {
        Logger::instance().debug("Received data from socket: " + data);
        return true;
    });
}

Result<bool> HttpServer::handle_closing(TcpSocket& socket) {
    socket.disconnect();
    return Result<bool>(true);
}

void HttpServer::handle_state_change(TcpSocket& socket) {
    switch (socket.get_connection_state()) {
        case TcpSocket::ConnectionState::IDLE:  
            socket.set_connection_state(TcpSocket::ConnectionState::READING);
        case TcpSocket::ConnectionState::READING: {
            auto message = socket.flush_recv();
            HttpRequest http_request(message);
            auto response = router.handle_request(http_request);
            socket.set_send_buffer(response.to_string());
            socket.set_connection_state(TcpSocket::ConnectionState::WRITING);
            break;
        }
        case TcpSocket::ConnectionState::WRITING:
            socket.set_connection_state(TcpSocket::ConnectionState::IDLE);
            break;
        case TcpSocket::ConnectionState::CLOSING:
            socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
            break;
    }
}

