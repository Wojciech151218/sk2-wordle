#include "server/web-socket/web_socket_server.h"
#include "server/server_method.h"



WebSocketServer::WebSocketServer() : TcpServer(), web_socket_pool(WebSocketPool::instance()) {
}

void WebSocketServer::run_loop() {
    while (true) {
        auto accept_result = socket.accept();
        if (accept_result.is_err()) break;
    }
}

void WebSocketServer::handle_client(TcpSocket* client_socket) {
    Logger& logger = Logger::instance();

    while (true) {
        auto request = client_socket->receive();
        if (request.is_err()) {
            client_socket->disconnect();
            break;
        }
        auto raw_request = request.unwrap();
        HttpRequest http_request(raw_request);

        if(http_request.get_method() == HttpMethod::GET && http_request.get_path() == "/ws") {

            logger.debug("Accepting web socket connection");
            auto web_socket_result = WebSocketConnection::accept(
                *client_socket,
                http_request
            ).log_debug<WebSocketConnection>();

            if(web_socket_result.is_err()) {
                auto error_response = HttpResponse::from_json(
                    Error("Failed to accept web socket", HttpStatusCode::BAD_REQUEST)
                );
                client_socket->send(error_response.to_string());
                client_socket->disconnect();
                break;
            }
            web_socket_pool.add(web_socket_result.unwrap());
            break;
        }
        else{
            logger.debug("Handling Web Socket request");
            auto web_socket_frame = WebSocketFrame(raw_request);
            auto json = web_socket_frame.json();

            if(json.is_err()) {
                logger.error(json.unwrap_err());
                continue;
            }
            web_socket_pool.broadcast_all(json.unwrap());
            
            //echo service todo

        }

    }

    web_socket_pool.remove(*client_socket);

}