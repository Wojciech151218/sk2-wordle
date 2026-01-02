#include "server/http/http_server.h"
#include "server/http/request_body.h"
#include "server/utils/logger.h"
#include "server/http/http_request.h"
#include "server/http/http_response.h"
#include <string>
#include "server/http/server_method.h"

HttpServer::HttpServer() : TcpServer() {
    router.add_method(
        ServerMethod<EmptyRequestBody>("/health", HttpMethod::GET, 
                [](const EmptyRequestBody& request) {
            return Result<nlohmann::json>(nlohmann::json{{"status", "ok"}});
        })
    );
}

HttpServer::~HttpServer() {}

void HttpServer::start(int port, std::string address) {
    Logger& logger = Logger::instance();
    logger.info("Starting HTTP server on " + address + ":" + std::to_string(port));
    router.log_methods();
    TcpServer::start(port, address);
}



std::string HttpServer::get_response_info(const HttpRequest& http_request,const HttpResponse& response, const TcpSocket& socket) const {
    return 
    method_to_string(http_request.get_method()) + 
    " " + http_request.get_path() + 
    " " + method_to_string(http_request.get_method()) + 
    " " + status_code_to_string(response.get_status_code()) + 
    " " + get_status_message(response.get_status_code()) + " " + socket.socket_info();
}


void HttpServer::on_client_connected(TcpSocket& client_socket) {
    client_socket.set_protocol_callback([&](std::string data) {
        if(data.empty()) return std::optional<std::string>();
        return std::optional<std::string>(data);
    });
}
void HttpServer::handle_message(TcpSocket& socket, std::string message) {
    HttpRequest request(message);
    HttpResponse response = router.handle_request(request);
    auto response_info = get_response_info(request, response, socket);
    if(!response.is_success()) {
        Logger::instance().error(response_info);
    }else{
        Logger::instance().info(response_info);
    }
    socket.set_send_buffer(response.to_string());
}

