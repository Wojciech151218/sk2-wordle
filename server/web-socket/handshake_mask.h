#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class WebSocketFrame; // Forward declaration

class HandshakeMask {
public:
    // Unmask payload using the masking key
    // According to RFC 6455: transformed-octet-i = original-octet-i XOR masking-key[i mod 4]
    static void unmask_payload(std::vector<uint8_t>& payload, const std::vector<uint8_t>& masking_key) {
        for (size_t i = 0; i < payload.size(); ++i) {
            payload[i] ^= masking_key[i % 4];
        }
    }

    // Mask payload using the masking key
    // Same operation as unmasking (XOR is symmetric)
    static void mask_payload(std::vector<uint8_t>& payload, const std::vector<uint8_t>& masking_key) {
        unmask_payload(payload, masking_key);
    }

    // Generate a masking key (4 bytes)
    // In production, this should use a cryptographically secure RNG
    static std::vector<uint8_t> generate_masking_key() {
        // TODO: Replace with proper random number generator
        return {0, 0, 0, 0};
    }

    // Friend class to allow access to WebSocket private members
    friend class WebSocketFrame;
};

