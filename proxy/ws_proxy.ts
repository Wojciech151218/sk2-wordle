import net from 'net';
import { WebSocketServer, WebSocket, type RawData } from 'ws';

type WordRequest = {
  method: string;
  word: string;
};

const TCP_HOST = process.env.TCP_HOST ?? '0.0.0.0';
const TCP_PORT = Number(process.env.TCP_PORT ?? 5000);
const WS_PORT = Number(process.env.WS_PORT ?? 5174);

const log = (...args: unknown[]) => {
  const timestamp = new Date().toISOString();
  console.log(`[proxy ${timestamp}]`, ...args);
};

const sendJson = (socket: WebSocket, payload: unknown) => {
  if (socket.readyState === WebSocket.OPEN) {
    socket.send(JSON.stringify(payload));
  }
};

const wss = new WebSocketServer({ port: WS_PORT });

wss.on('listening', () => {
  log(`WebSocket proxy listening on ws://0.0.0.0:${WS_PORT}`);
  log(`Proxying TCP traffic to ${TCP_HOST}:${TCP_PORT}`);
});

wss.on('connection', (ws, req) => {
  log('WebSocket client connected', req.socket.remoteAddress);

  let tcpClient: net.Socket | null = null;
  let wsClosedByClient = false;
  let reconnectTimeout: NodeJS.Timeout | null = null;

  const createTcpConnection = () => {
    if (wsClosedByClient || ws.readyState !== WebSocket.OPEN) {
      return;
    }

    // Clean up existing connection if any
    if (tcpClient && !tcpClient.destroyed) {
      tcpClient.removeAllListeners();
      tcpClient.destroy();
    }

    tcpClient = net.createConnection(
      { host: TCP_HOST, port: TCP_PORT },
      () => {
        log('TCP connection established for client');
        sendJson(ws, { type: 'tcp_connected' });
      },
    );

    tcpClient.on('data', (chunk) => {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(chunk.toString());
      }
    });

    tcpClient.on('error', (err) => {
      log('TCP error', err.message);
      sendJson(ws, { type: 'error', source: 'tcp', message: err.message });
      // Don't close WebSocket on TCP error - try to reconnect TCP
      if (tcpClient) {
        tcpClient.removeAllListeners();
        tcpClient.destroy();
        tcpClient = null;
      }
      // Attempt to reconnect TCP after a short delay
      if (reconnectTimeout) {
        clearTimeout(reconnectTimeout);
      }
      reconnectTimeout = setTimeout(() => {
        if (!wsClosedByClient && ws.readyState === WebSocket.OPEN) {
          log('Attempting to reconnect TCP...');
          createTcpConnection();
        }
      }, 1000);
    });

    tcpClient.on('close', (hadError) => {
      log('TCP connection closed', hadError ? '(with error)' : '(normal)');
      sendJson(ws, { type: 'tcp_closed', hadError });
      tcpClient = null;
      // Only close WebSocket if client explicitly closed it
      // Otherwise, try to reconnect TCP
      if (!wsClosedByClient && ws.readyState === WebSocket.OPEN) {
        log('TCP closed unexpectedly, attempting reconnect...');
        if (reconnectTimeout) {
          clearTimeout(reconnectTimeout);
        }
        reconnectTimeout = setTimeout(() => {
          if (!wsClosedByClient && ws.readyState === WebSocket.OPEN) {
            createTcpConnection();
          }
        }, 1000);
      }
    });
  };

  createTcpConnection();

  const rawToString = (raw: RawData): string => {
    if (typeof raw === 'string') return raw;
    if (Buffer.isBuffer(raw)) return raw.toString('utf-8');
    if (Array.isArray(raw)) return Buffer.concat(raw).toString('utf-8');
    return Buffer.from(raw).toString('utf-8');
  };

  ws.on('message', (raw) => {
    if (!tcpClient || tcpClient.destroyed) {
      sendJson(ws, {
        type: 'error',
        source: 'proxy',
        message: 'TCP connection not available, reconnecting...',
      });
      // Try to recreate TCP connection
      createTcpConnection();
      return;
    }

    try {
      const parsed = JSON.parse(rawToString(raw)) as WordRequest;

      if (!parsed?.method || typeof parsed.word !== 'string') {
        sendJson(ws, {
          type: 'error',
          source: 'proxy',
          message: 'Invalid payload format',
        });
        return;
      }

      tcpClient.write(JSON.stringify(parsed));
    } catch (err) {
      const message =
        err instanceof Error ? err.message : 'Unknown parse error';
      log('Failed to handle message', message);
      sendJson(ws, { type: 'error', source: 'proxy', message });
    }
  });

  ws.on('close', () => {
    wsClosedByClient = true;
    log('WebSocket closed by client');
    if (reconnectTimeout) {
      clearTimeout(reconnectTimeout);
    }
    if (tcpClient && !tcpClient.destroyed) {
      tcpClient.end();
    }
  });

  ws.on('error', (err) => {
    log('WebSocket error', err.message);
    sendJson(ws, { type: 'error', source: 'ws', message: err.message });
  });
});

wss.on('error', (err) => {
  log('WebSocket server error', err.message);
});

