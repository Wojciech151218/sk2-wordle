#include "server/web-socket/web_socket_frame.h"
#include "server/web-socket/handshake_mask.h"
#include "server/utils/error.h"
#include <cstring>
#include <stdexcept>

// Constructor: Parse raw WebSocket frame
WebSocketFrame::WebSocketFrame(const std::vector<uint8_t>& raw_frame) {
    if (raw_frame.size() < 2) {
        throw std::runtime_error("WebSocket frame too short");
    }

    // Parse first byte: FIN + RSV + OpCode
    fin = (raw_frame[0] & 0x80) != 0;
    opcode = static_cast<OpCode>(raw_frame[0] & 0x0F);

    // Parse second byte: MASK + Payload length
    masked = (raw_frame[1] & 0x80) != 0;
    uint8_t payload_len_byte = raw_frame[1] & 0x7F;

    size_t header_size = 2;
    uint64_t payload_length = 0;

    // Determine payload length
    if (payload_len_byte < 126) {
        payload_length = payload_len_byte;
    } else if (payload_len_byte == 126) {
        if (raw_frame.size() < 4) {
            throw std::runtime_error("WebSocket frame too short for 16-bit length");
        }
        payload_length = (static_cast<uint64_t>(raw_frame[2]) << 8) | raw_frame[3];
        header_size = 4;
    } else if (payload_len_byte == 127) {
        if (raw_frame.size() < 10) {
            throw std::runtime_error("WebSocket frame too short for 64-bit length");
        }
        payload_length = 0;
        for (int i = 0; i < 8; ++i) {
            payload_length = (payload_length << 8) | raw_frame[2 + i];
        }
        header_size = 10;
    }

    // Extract masking key if present
    std::vector<uint8_t> masking_key;
    if (masked) {
        if (raw_frame.size() < header_size + 4) {
            throw std::runtime_error("WebSocket frame too short for masking key");
        }
        masking_key.assign(raw_frame.begin() + header_size, raw_frame.begin() + header_size + 4);
        header_size += 4;
    }

    // Extract and unmask payload
    if (raw_frame.size() < header_size + payload_length) {
        throw std::runtime_error("WebSocket frame payload incomplete");
    }

    payload.assign(raw_frame.begin() + header_size, raw_frame.begin() + header_size + payload_length);

    // Unmask payload if masked
    if (masked) {
        HandshakeMask::unmask_payload(payload, masking_key);
    }

    // Parse JSON if text frame
    if (opcode == OpCode::TEXT) {
        try {
            std::string payload_str(payload.begin(), payload.end());
            json_payload = nlohmann::json::parse(payload_str);
        } catch (const nlohmann::json::parse_error&) {
            // JSON parsing failed, json_payload remains empty
        }
    }

    // Parse close frame
    if (opcode == OpCode::CLOSE && payload.size() >= 2) {
        close_code = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];
        if (payload.size() > 2) {
            close_reason.assign(payload.begin() + 2, payload.end());
        }
    }
}

WebSocketFrame::WebSocketFrame(const std::string& raw_frame) {
    std::vector<uint8_t> raw_frame_vec(raw_frame.begin(), raw_frame.end());
    *this = WebSocketFrame(raw_frame_vec);
}

// Static factory methods
WebSocketFrame WebSocketFrame::text(const nlohmann::json& json) {
    WebSocketFrame ws;
    ws.fin = true;
    ws.masked = false;
    ws.opcode = OpCode::TEXT;
    ws.json_payload = json;
    
    std::string json_str = json.dump();
    ws.payload.assign(json_str.begin(), json_str.end());
    
    return ws;
}

WebSocketFrame WebSocketFrame::binary(const std::vector<uint8_t>& data) {
    WebSocketFrame ws;
    ws.fin = true;
    ws.masked = false;
    ws.opcode = OpCode::BINARY;
    ws.payload = data;
    
    return ws;
}

WebSocketFrame WebSocketFrame::ping() {
    WebSocketFrame ws;
    ws.fin = true;
    ws.masked = false;
    ws.opcode = OpCode::PING;
    ws.payload.clear();
    
    return ws;
}

WebSocketFrame WebSocketFrame::pong() {
    WebSocketFrame ws;
    ws.fin = true;
    ws.masked = false;
    ws.opcode = OpCode::PONG;
    ws.payload.clear();
    
    return ws;
}

WebSocketFrame WebSocketFrame::close(uint16_t code, const std::string& reason) {
    WebSocketFrame ws;
    ws.fin = true;
    ws.masked = false;
    ws.opcode = OpCode::CLOSE;
    ws.close_code = code;
    ws.close_reason = reason;
    
    // Build close frame payload: 2-byte code + reason
    ws.payload.clear();
    ws.payload.push_back(static_cast<uint8_t>((code >> 8) & 0xFF));
    ws.payload.push_back(static_cast<uint8_t>(code & 0xFF));
    ws.payload.insert(ws.payload.end(), reason.begin(), reason.end());
    
    return ws;
}

// Query methods
bool WebSocketFrame::is_text() const {
    return opcode == OpCode::TEXT;
}

bool WebSocketFrame::is_binary() const {
    return opcode == OpCode::BINARY;
}

bool WebSocketFrame::is_close() const {
    return opcode == OpCode::CLOSE;
}

bool WebSocketFrame::is_ping() const {
    return opcode == OpCode::PING;
}

bool WebSocketFrame::is_pong() const {
    return opcode == OpCode::PONG;
}

WebSocketFrame::OpCode WebSocketFrame::get_opcode() const {
    return opcode;
}

bool WebSocketFrame::get_fin() const {
    return fin;
}

const std::vector<uint8_t>& WebSocketFrame::raw_payload() const {
    return payload;
}

Result<nlohmann::json> WebSocketFrame::json() const {
    if (opcode != OpCode::TEXT) {
        return Result<nlohmann::json>(Error("WebSocket frame is not a text frame"));
    }
    
    if (json_payload.has_value()) {
        return Result<nlohmann::json>(json_payload.value());
    }
    
    // Try to parse JSON from payload
    try {
        std::string payload_str(payload.begin(), payload.end());
        nlohmann::json json = nlohmann::json::parse(payload_str);
        return Result<nlohmann::json>(json);
    } catch (const nlohmann::json::parse_error& e) {
        return Result<nlohmann::json>(Error("Failed to parse JSON: " + std::string(e.what())));
    }
}

// Serialize to WebSocket frame
std::vector<uint8_t> WebSocketFrame::to_frame() const {
    std::vector<uint8_t> frame;
    
    // First byte: FIN + RSV + OpCode
    uint8_t first_byte = static_cast<uint8_t>(opcode);
    if (fin) {
        first_byte |= 0x80;
    }
    frame.push_back(first_byte);
    
    // Second byte: MASK + Payload length
    uint64_t payload_len = payload.size();
    uint8_t second_byte = 0;
    
    if (masked) {
        second_byte |= 0x80;
    }
    
    if (payload_len < 126) {
        second_byte |= static_cast<uint8_t>(payload_len);
        frame.push_back(second_byte);
    } else if (payload_len < 65536) {
        second_byte |= 126;
        frame.push_back(second_byte);
        // 16-bit length (big-endian)
        frame.push_back(static_cast<uint8_t>((payload_len >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_len & 0xFF));
    } else {
        second_byte |= 127;
        frame.push_back(second_byte);
        // 64-bit length (big-endian)
        for (int i = 7; i >= 0; --i) {
            frame.push_back(static_cast<uint8_t>((payload_len >> (i * 8)) & 0xFF));
        }
    }
    
    // Add masking key if masked (for client-to-server frames)
    // Note: Server-to-client frames should not be masked per WebSocket spec
    std::vector<uint8_t> masking_key;
    if (masked) {
        masking_key = HandshakeMask::generate_masking_key();
        frame.insert(frame.end(), masking_key.begin(), masking_key.end());
    }
    
    // Add payload (unmasked for server-to-client, will be masked if needed)
    std::vector<uint8_t> payload_to_add = payload;
    
    // If masked, apply masking to payload before adding
    if (masked) {
        HandshakeMask::mask_payload(payload_to_add, masking_key);
    }
    
    frame.insert(frame.end(), payload_to_add.begin(), payload_to_add.end());
    
    return frame;
}

