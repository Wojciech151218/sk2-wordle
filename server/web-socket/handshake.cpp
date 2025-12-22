#include "server/web-socket/handshake.h"
#include "server/http/http_response.h"
#include "server/http/http_request.h"
#include "server/utils/result.h"
#include "server/utils/error.h"
#include <iostream>


std::string base64_enncode(const unsigned char* input, int length) {
    BIO* bmem = nullptr;
    BIO* b64 = nullptr;
    BUF_MEM* bptr = nullptr;

    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // no newlines
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(bmem, &bptr);

    std::string output(bptr->data, bptr->length);
    BIO_free_all(b64);

    return output;
}

std::string compute_web_socket_accept(std::string client_key) {
    static const std::string GUID =
        "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    // 1. Concatenate key + GUID
    std::string combined = client_key + GUID;

    // 2. SHA-1 hash
    unsigned char sha1Result[SHA_DIGEST_LENGTH];
    SHA1(
        reinterpret_cast<const unsigned char*>(combined.data()),
        combined.size(),
        sha1Result
    );

    // 3. Base64 encode
    return base64_enncode(sha1Result, SHA_DIGEST_LENGTH);
}

HttpResponse handshake_response(std::string key){
    std::string response_key = compute_web_socket_accept(key);

    return HttpResponse(
        std::nullopt,
        HttpVersion::HTTP_1_1,
        HttpStatusCode::SWITCHING_PROTOCOLS
    )
    .add_header(HttpHeader("Upgrade", "websocket"))
    .add_header(HttpHeader("Connection", "Upgrade"))
    .add_header(HttpHeader("Sec-WebSocket-Accept",response_key));
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

