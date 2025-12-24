#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include "nlohmann/json.hpp"
#include "server/utils/result.h"

class HandshakeMask; // Forward declaration

class WebSocketFrame {
public:
    enum class OpCode : uint8_t {
        CONTINUATION = 0x0,
        TEXT         = 0x1,
        BINARY       = 0x2,
        CLOSE        = 0x8,
        PING         = 0x9,
        PONG         = 0xA
    };

private:
    bool fin = true;
    bool masked = false;
    OpCode opcode = OpCode::TEXT;

    std::vector<uint8_t> payload;    
    std::optional<nlohmann::json> json_payload;

    uint16_t close_code = 1000;
    std::string close_reason;

    // Private default constructor for static factory methods
    WebSocketFrame() = default;

    // Friend class for masking operations
    friend class HandshakeMask;

public:
    WebSocketFrame(const std::vector<uint8_t>& raw_frame);
    WebSocketFrame(const std::string& raw_frame);

    static WebSocketFrame text(const nlohmann::json& json);
    static WebSocketFrame binary(const std::vector<uint8_t>& data);
    static WebSocketFrame ping();
    static WebSocketFrame pong();
    static WebSocketFrame close(uint16_t code = 1000,
                           const std::string& reason = "");

    bool is_text() const;
    bool is_binary() const;
    bool is_close() const;
    bool is_ping() const;
    bool is_pong() const;

    OpCode get_opcode() const;
    bool get_fin() const;

    const std::vector<uint8_t>& raw_payload() const;

    Result<nlohmann::json> json() const;

    std::vector<uint8_t> to_frame() const;
};
