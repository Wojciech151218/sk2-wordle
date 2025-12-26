#include "server/web-socket/web_socket_server.h"
#include "server/utils/result.h"
#include "server/web-socket/handshake.h"
#include "server/http/http_request.h"
#include <string>
#include "server/web-socket/web_socket_frame.h"


WebSocketServer::WebSocketServer() : TcpServer() {

    WebSocketPool::instance().set_connections(&connections);
}


void WebSocketServer::start(int port, std::string address) {
    Logger::instance().info("Starting WebSocket server on " + address + ":" + std::to_string(port));
    TcpServer::start(port, address);
}

Result<bool> WebSocketServer::handle_connected(TcpSocket& socket) {
    socket.set_protocol_callback([&](std::string data) {
        return true;
    });
    return TcpServer::handle_connected(socket);
}

void WebSocketServer::handle_state_change(TcpSocket& socket) {
    switch (socket.get_connection_state()) {
        case TcpSocket::ConnectionState::CONNECTED: {

            auto handshake = socket.flush_recv();
            HttpRequest http_request(handshake);

            if (http_request.get_method() != HttpMethod::GET || http_request.get_path() != "/ws") {
                socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
                return;
            }

            auto client_key = handshake_request(http_request);
            if (client_key.is_err()) {
                socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
                return;
            }
            auto response = handshake_response(client_key.unwrap());
            socket.set_send_buffer(response.to_string());
            socket.set_connection_state(TcpSocket::ConnectionState::WRITING);
            Logger::instance().info("Handshake successful for connection: " + socket.socket_info());
            socket.set_protocol_callback([&](std::string data) {
                //todo: handle websocket frames
                return true;
            });
            break;
        }
        case TcpSocket::ConnectionState::WRITING:
            socket.set_connection_state(TcpSocket::ConnectionState::IDLE);
            break;
            
        case TcpSocket::ConnectionState::IDLE:
        case TcpSocket::ConnectionState::READING: {
            auto message = socket.flush_recv();
            auto frame = WebSocketFrame::from_raw_data(message).log_error();
            if (frame.is_err()) {
                socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
                return;
            }
            auto payload = frame.unwrap().payload_as_string();
            Logger::instance().debug("Received message: " + payload);
            
            socket.set_connection_state(TcpSocket::ConnectionState::IDLE);
            break;
        }
        
        case TcpSocket::ConnectionState::CLOSING:
            socket.set_connection_state(TcpSocket::ConnectionState::CLOSING);
            break;
    }
}