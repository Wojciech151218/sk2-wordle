#include "server/web-socket/web_socket_server.h"
#include "server/utils/result.h"
#include "server/web-socket/handshake.h"
#include "server/http/http_request.h"
#include <string>
#include "server/web-socket/web_socket_frame.h"


WebSocketServer::WebSocketServer() : TcpServer() {
    set_client_timeout(std::chrono::seconds::max());
    WebSocketPool::instance().set_connections(connections);
}


void WebSocketServer::start(int port, std::string address) {
    Logger::instance().info("Starting WebSocket server on " + address + ":" + std::to_string(port));
    TcpServer::start(port, address);
}

Result<std::string> WebSocketServer::handle_message(TcpSocket& socket, std::string message) {
    auto handshake_status = socket.get_metadata("handshake_status");
    if(!handshake_status.has_value()) {
        HttpRequest request(message);

        bool is_get_request = request.get_method() == HttpMethod::GET;
        bool is_ws_path = request.get_path() == "/ws";
        if(!is_get_request || !is_ws_path) {

            HttpResponse response =  is_get_request ? 
            HttpResponse::from_json(Error("Invalid request", HttpStatusCode::METHOD_NOT_ALLOWED)) 
            : HttpResponse::from_json(Error("Invalid request", HttpStatusCode::NOT_FOUND));
            return Result<std::string>(response.to_string());
        }
        auto handshake_key = handshake_request(request);
        if(handshake_key.is_err()) {
            HttpResponse response = HttpResponse::from_json(Error("Invalid request", HttpStatusCode::BAD_REQUEST));
            return Result<std::string>(response.to_string());
        }

        auto response = handshake_response(handshake_key.unwrap());
        socket.set_metadata("handshake_status", true);
        socket.set_protocol_callback([&](std::string data) {
            //should switch to websocket frame handling
            if (data.empty()) return std::optional<std::string>();
            return std::optional<std::string>(data);
        });
        return Result<std::string>(response.to_string());

    }else if(handshake_status) {
        auto frame_result = WebSocketFrame::from_raw_data(message);
        if(frame_result.is_err()) {
            WebSocketFrame response = WebSocketFrame::close(WsCloseCode::PROTOCOL_ERROR);
            Logger::instance().error("Failed to parse frame from client " + socket.socket_info());
            socket.set_metadata("half_closed", true);
            return Result<std::string>(response.to_string());
        }
        auto frame = frame_result.unwrap();
        if(frame.opcode == WsOpcode::Close) {
            WebSocketFrame response = WebSocketFrame::close(WsCloseCode::NORMAL_CLOSURE);
            socket.set_send_buffer(response.to_string());
            socket.set_metadata("half_closed", true);
            return Result<std::string>(response.to_string());
        }

        auto frame_payload = frame.payload_as_string();
        Logger::instance().debug("Received frame from client " + frame_payload);

        //echo todo
        auto response = WebSocketFrame::text(frame_payload);
        return Result<std::string>(response.to_string());

    }else{
        Logger::instance().error("Handshake failed for client " + socket.socket_info());
        return Error("Unexpected message handler status");
    }
}

void WebSocketServer::on_client_connected(TcpSocket& client_socket) {
    client_socket.set_protocol_callback([&](std::string data) {
        // should add http callback
        if (data.empty()) return std::optional<std::string>();
        return std::optional<std::string>(data);
    });
}
   
