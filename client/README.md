# Wordle React Client

A polished Vite + React + TypeScript client for the TCP Wordle backend. The UI mirrors classic Wordle interactions, adds buttery animations via Framer Motion, and communicates through an HTTP REST API proxy.

## Getting started

```bash
# From repo root
cd client
npm install

# (optional) configure the API endpoint
echo "VITE_API_URL=http://localhost:5174/api" > .env.local
```

`VITE_API_URL` defaults to `http://localhost:5174/api`. Override it via `.env.local` if the proxy runs elsewhere.

### Running the stack

In two terminals:

```bash
# 1) HTTP → TCP proxy at repo root
npm run proxy:start --prefix ../proxy

# 2) React dev server
npm run dev
```

Visit http://localhost:5173 to play. The proxy maintains a persistent TCP connection to `0.0.0.0:5000` and exposes a REST API endpoint at `/api`.

### API Endpoint

**GET** `/api?method=word&word=hello`

The proxy translates HTTP GET requests with query parameters into JSON payloads sent to the TCP server:

```json
{ "method": "word", "word": "hello" }
```

Responses are returned as JSON:

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

## Legacy TCP test client

The original Node TCP helper now lives in `scripts/tcp_client.js`. Run it with:

```bash
node scripts/tcp_client.js
```
