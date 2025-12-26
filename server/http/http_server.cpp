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

Result<bool> HttpServer::handle_connected(TcpSocket& socket) {
    socket.set_protocol_callback([&](std::string data) {
        return true;
    });
    return TcpServer::handle_connected(socket);
}

std::string HttpServer::get_response_info(std::string path,const HttpResponse& response, const TcpSocket& socket) const {
    return path + 
    " " + status_code_to_string(response.get_status_code()) + 
    " " + get_status_message(response.get_status_code()) + " " + socket.socket_info();
}
void HttpServer::handle_state_change(TcpSocket& socket) {
    switch (socket.get_connection_state()) {
        case TcpSocket::ConnectionState::CONNECTED:
        case TcpSocket::ConnectionState::IDLE:  
            socket.set_connection_state(TcpSocket::ConnectionState::READING);
        case TcpSocket::ConnectionState::READING: {
            auto message = socket.flush_recv();
            HttpRequest http_request(message);
            Logger::instance().debug("Received request: " + http_request.to_string());
            auto response = router.handle_request(http_request);
            Logger::instance().debug("Response: " + response.to_string());

            if(!response.is_success()) {
                Logger::instance().error(get_response_info(http_request.get_path(), response, socket));
            }else{
                Logger::instance().info(get_response_info(http_request.get_path(), response, socket));
            }
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

