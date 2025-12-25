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
    NormalClosure      = 1000,
    GoingAway          = 1001,
    ProtocolError      = 1002,
    UnsupportedData    = 1003,
    NoStatusRcvd       = 1005,
    AbnormalClosure    = 1006,
    InvalidPayload     = 1007,
    PolicyViolation    = 1008,
    MessageTooBig      = 1009,
    MandatoryExt       = 1010,
    InternalError      = 1011
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
            WsCloseCode code = WsCloseCode::NormalClosure,
            const std::string& reason = ""
        );
        Result<WebSocketFrame> unmask();
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
    