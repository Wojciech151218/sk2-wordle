#include "server/web-socket/web_socket_connection.h"
#include "server/web-socket/web_socket_frame.h"
#include "server/web-socket/handshake.h"
#include "server/web-socket/handshake_mask.h"

WebSocketConnection::WebSocketConnection(TcpSocket socket) : socket(socket) {
}

WebSocketConnection::~WebSocketConnection() {
    close();
}


Result<WebSocketConnection> WebSocketConnection::accept(TcpSocket& socket,HttpRequest& request){
    auto key = handshake_request(request);
    if (key.is_err()) {
        return Error("Failed to handshake");
    }

    auto response_result = socket.send(handshake_response(key.unwrap()).to_string());
    if (response_result.is_err()) {
        return Error("Failed to send handshake response");
    }

    return WebSocketConnection(socket);
}

Result<void*> WebSocketConnection::send(const nlohmann::json& json) {

    auto frame_json = WebSocketFrame::text(json).json();
    if (frame_json.is_err()) {
        return Error("Failed to convert JSON to frame");
    }
    return socket.send(
        frame_json.unwrap().dump()
    ).finally<void*>([&]() {
        return nullptr;
    });
}

Result<WebSocketFrame> WebSocketConnection::receive(){
    return socket.receive()
    .chain<nlohmann::json>([&](auto data ) {
        try {
            return Result<nlohmann::json>(nlohmann::json::parse(data));
        } catch (const nlohmann::json::parse_error& e) {
            return Result<nlohmann::json>(
                Error("Failed to parse JSON: " + std::string(e.what()))
            );
        }
    })
    .chain<WebSocketFrame>([&](auto json ) {
        return WebSocketFrame::text(json);
    });
}

Result<void*> WebSocketConnection::close() {
    return socket.disconnect()
    .finally<void*>([&]() {
        return nullptr;
    });
}


