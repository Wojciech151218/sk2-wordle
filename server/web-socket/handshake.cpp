#include "server/web-socket/handshake.h"
#include "server/http/http_response.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"
#include "server/utils/error.h"

HttpResponse handshake_response(const std::string& key){

    return HttpResponse(
        std::nullopt,
        HttpVersion::HTTP_1_1,
        HttpStatusCode::SWITCHING_PROTOCOLS
    )
    .add_header(HttpHeader("Upgrade", "websocket"))
    .add_header(HttpHeader("Connection", "Upgrade"))
    .add_header(HttpHeader("Sec-WebSocket-Accept", key));
}


Result<std::string> handshake_request(const HttpRequest& request){
    auto headers = request.get_headers();
    auto upgrade = headers.find("Upgrade");
    auto connection = headers.find("Connection");
    auto sec_websocket_key = headers.find("Sec-WebSocket-Key");
    if (upgrade == headers.end() || upgrade->second != "websocket"){
        return Error("Upgrade header missing");
    }
    if (connection == headers.end() || connection->second != "Upgrade"){
        return Error("Connection header missing");
    }
    if (sec_websocket_key== headers.end()){
        return Error("Sec-WebSocket-Key header missing");
    }


    return sec_websocket_key->second;
}

