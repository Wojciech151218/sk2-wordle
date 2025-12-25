#include "web_socket_frame.h"
#include <stdexcept>
#include <cstring>
#include <string>
#include <arpa/inet.h>

// ============================================================================
// Factory: Parse raw WebSocket frame data
// ============================================================================

Result<WebSocketFrame> WebSocketFrame::from_raw_data(const std::vector<uint8_t>& data) {
    if (data.size() < 2) {
        return Error("WebSocket frame too short (minimum 2 bytes)");
    }

    WebSocketFrame frame;
    size_t offset = 0;

    // Byte 0: FIN, RSV, Opcode
    uint8_t byte0 = data[offset++];
    frame.fin = (byte0 & 0x80) != 0;
    frame.rsv1 = (byte0 & 0x40) != 0;
    frame.rsv2 = (byte0 & 0x20) != 0;
    frame.rsv3 = (byte0 & 0x10) != 0;
    frame.opcode = static_cast<WsOpcode>(byte0 & 0x0F);

    // Byte 1: MASK, Payload length
    uint8_t byte1 = data[offset++];
    frame.masked = (byte1 & 0x80) != 0;
    uint8_t payload_len = byte1 & 0x7F;

    // Extended payload length
    if (payload_len == 126) {
        if (data.size() < offset + 2) {
            return Error("WebSocket frame truncated at extended payload length");
        }
        frame.payload_length = (static_cast<uint64_t>(data[offset]) << 8) | data[offset + 1];
        offset += 2;
    } else if (payload_len == 127) {
        if (data.size() < offset + 8) {
            return Error("WebSocket frame truncated at extended payload length");
        }
        frame.payload_length = 0;
        for (int i = 0; i < 8; ++i) {
            frame.payload_length = (frame.payload_length << 8) | data[offset + i];
        }
        offset += 8;
    } else {
        frame.payload_length = payload_len;
    }

    // Masking key (if present)
    if (frame.masked) {
        if (data.size() < offset + 4) {
            return Error("WebSocket frame truncated at masking key");
        }
        std::copy(data.begin() + offset, data.begin() + offset + 4, frame.masking_key.begin());
        offset += 4;
    }

    // Payload data
    if (data.size() < offset + frame.payload_length) {
        return Error("WebSocket frame truncated at payload");
    }

    frame.payload.assign(data.begin() + offset, data.begin() + offset + frame.payload_length);

    // Unmask payload if needed
    if (frame.masked) {
        for (size_t i = 0; i < frame.payload.size(); ++i) {
            frame.payload[i] ^= frame.masking_key[i % 4];
        }
    }

    return frame;
}

Result<WebSocketFrame> WebSocketFrame::from_raw_data(std::string_view data) {
    std::vector<uint8_t> bytes(data.begin(), data.end());
    return from_raw_data(bytes);
}

// ============================================================================
// Factory: Create specific frame types
// ============================================================================

WebSocketFrame WebSocketFrame::text(std::string_view msg) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WsOpcode::Text;
    frame.payload.assign(msg.begin(), msg.end());
    frame.payload_length = frame.payload.size();
    return frame;
}

WebSocketFrame WebSocketFrame::binary(const std::vector<uint8_t>& data) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WsOpcode::Binary;
    frame.payload = data;
    frame.payload_length = frame.payload.size();
    return frame;
}

WebSocketFrame WebSocketFrame::ping(const std::vector<uint8_t>& payload) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WsOpcode::Ping;
    frame.payload = payload;
    frame.payload_length = frame.payload.size();
    return frame;
}

WebSocketFrame WebSocketFrame::pong(const std::vector<uint8_t>& payload) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WsOpcode::Pong;
    frame.payload = payload;
    frame.payload_length = frame.payload.size();
    return frame;
}

WebSocketFrame WebSocketFrame::close(WsCloseCode code, const std::string& reason) {
    WebSocketFrame frame;
    frame.fin = true;
    frame.opcode = WsOpcode::Close;
    
    // Close frame payload: 2-byte code + optional reason
    uint16_t code_network = htons(static_cast<uint16_t>(code));
    frame.payload.resize(2 + reason.size());
    std::memcpy(frame.payload.data(), &code_network, 2);
    if (!reason.empty()) {
        std::copy(reason.begin(), reason.end(), frame.payload.begin() + 2);
    }
    frame.payload_length = frame.payload.size();
    
    return frame;
}

// ============================================================================
// Masking/Unmasking
// ============================================================================

Result<WebSocketFrame> WebSocketFrame::unmask() {
    if (!masked) {
        return Error("WebSocket frame is not masked");
    }

    WebSocketFrame unmasked_frame = *this;
    
    // Apply XOR mask to payload
    for (size_t i = 0; i < unmasked_frame.payload.size(); ++i) {
        unmasked_frame.payload[i] ^= masking_key[i % 4];
    }
    
    unmasked_frame.masked = false;
    
    return unmasked_frame;
}

// ============================================================================
// Serialization
// ============================================================================

std::vector<uint8_t> WebSocketFrame::to_bytes() const {
    std::vector<uint8_t> result;
    
    // Byte 0: FIN, RSV, Opcode
    uint8_t byte0 = static_cast<uint8_t>(opcode);
    if (fin) byte0 |= 0x80;
    if (rsv1) byte0 |= 0x40;
    if (rsv2) byte0 |= 0x20;
    if (rsv3) byte0 |= 0x10;
    result.push_back(byte0);

    // Byte 1: MASK, Payload length
    uint8_t byte1 = 0;
    if (masked) byte1 |= 0x80;

    size_t len = payload.size();
    if (len < 126) {
        byte1 |= static_cast<uint8_t>(len);
        result.push_back(byte1);
    } else if (len <= 0xFFFF) {
        byte1 |= 126;
        result.push_back(byte1);
        result.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
        result.push_back(static_cast<uint8_t>(len & 0xFF));
    } else {
        byte1 |= 127;
        result.push_back(byte1);
        for (int i = 7; i >= 0; --i) {
            result.push_back(static_cast<uint8_t>((len >> (i * 8)) & 0xFF));
        }
    }

    // Masking key (if masked)
    if (masked) {
        result.insert(result.end(), masking_key.begin(), masking_key.end());
    }

    // Payload (already masked if needed)
    result.insert(result.end(), payload.begin(), payload.end());

    return result;
}

std::string WebSocketFrame::to_string() const {
    auto bytes = to_bytes();
    return std::string(bytes.begin(), bytes.end());
}

// ============================================================================
// Utilities
// ============================================================================

bool WebSocketFrame::is_control() const {
    return static_cast<uint8_t>(opcode) >= 0x8;
}

bool WebSocketFrame::is_data() const {
    return !is_control();
}

bool WebSocketFrame::is_fragment() const {
    return !fin;
}

std::string WebSocketFrame::payload_as_string() const {
    return std::string(payload.begin(), payload.end());
}

Result<nlohmann::json> WebSocketFrame::to_json() const {
    try {
        return nlohmann::json::parse(payload_as_string());
    } catch (const nlohmann::json::parse_error& e) {
        return Error("Failed to parse JSON: " + std::string(e.what()));
    }
}



