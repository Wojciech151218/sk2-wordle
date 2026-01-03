#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include <string>
#include <string_view>
#include "server/utils/result.h"

#include "nlohmann/json.hpp"

enum class WsOpcode : uint8_t {
    Continuation = 0x0,
    Text         = 0x1,
    Binary       = 0x2,
    Close        = 0x8,
    Ping         = 0x9,
    Pong         = 0xA
};

enum class WsCloseCode : uint16_t {
    NORMAL_CLOSURE     = 1000,
    GOING_AWAY         = 1001,
    PROTOCOL_ERROR     = 1002,
    UNSUPPORTED_DATA   = 1003,
    NO_STATUS_RCVD     = 1005,
    ABNORMAL_CLOSURE   = 1006,
    INVALID_PAYLOAD    = 1007,
    POLICY_VIOLATION   = 1008,
    MESSAGE_TOO_BIG    = 1009,
    MANDATORY_EXT      = 1010,
    INTERNAL_ERROR     = 1011
};

class WebSocketFrame {
    public:
        // --- Header fields ---
        bool fin = true;
        bool rsv1 = false;
        bool rsv2 = false;
        bool rsv3 = false;
    
        WsOpcode opcode = WsOpcode::Text;
        bool masked = false;
    
        uint64_t payload_length = 0;
        std::array<uint8_t, 4> masking_key{};
    
        // --- Payload ---
        std::vector<uint8_t> payload;
    
        // --- Construction ---
        WebSocketFrame() = default;
    
        // Factories
        static Result<WebSocketFrame> from_raw_data(const std::vector<uint8_t>& data);
        static Result<WebSocketFrame> from_raw_data(std::string_view data);
    
        static WebSocketFrame text(std::string_view msg);
        static WebSocketFrame binary(const std::vector<uint8_t>& data);
        static WebSocketFrame ping(const std::vector<uint8_t>& payload = {});
        static WebSocketFrame pong(const std::vector<uint8_t>& payload = {});

        static WebSocketFrame close(
            WsCloseCode code = WsCloseCode::NORMAL_CLOSURE,
            const std::string& reason = ""
        );
        Result<nlohmann::json> to_json() const;

        // Serialization
        std::vector<uint8_t> to_bytes() const;
        std::string to_string() const;
    
        // Utilities
        bool is_control() const;
        bool is_data() const;
        bool is_fragment() const;
    
        std::string payload_as_string() const;
    


        
    };
    