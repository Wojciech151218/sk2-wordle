# HTTP → TCP Proxy

A lightweight HTTP REST API proxy that translates HTTP requests to TCP protocol for the Wordle backend server.

## Overview

The proxy maintains a persistent TCP connection to the backend server and exposes a simple HTTP REST API endpoint. It handles connection management, request/response translation, and automatic reconnection.

## Configuration

Environment variables:

- `TCP_HOST` - Backend TCP server host (default: `0.0.0.0`)
- `TCP_PORT` - Backend TCP server port (default: `5000`)
- `HTTP_PORT` - HTTP proxy server port (default: `5174`)

## API Endpoint

**GET** `/api`

Query parameters:
- `method` (required) - The method name (e.g., "word")
- `word` (required) - The word to guess

Example:
```
GET /api?method=word&word=hello
```

Response:
```json
{
  "letters_per_word": 5,
  "max_words": 5,
  "status": "ok",
  "words": [
    {
      "letters": [
        { "letter": "h", "type": "green" },
        { "letter": "e", "type": "green" },
        { "letter": "l", "type": "yellow" },
        { "letter": "l", "type": "green" },
        { "letter": "o", "type": "gray" }
      ]
    }
  ]
}
```

## Running

```bash
# Development mode (with watch)
npm run dev

# Production mode
npm run start
```

The proxy will listen on `http://0.0.0.0:5174` by default.

## Architecture

- **TCP Connection Pool**: Maintains a single persistent TCP connection to the backend
- **Request Queue**: Handles concurrent requests by queuing them if needed
- **Auto-reconnect**: Automatically reconnects TCP connection on errors or disconnects
- **CORS**: Includes CORS headers to allow browser requests

