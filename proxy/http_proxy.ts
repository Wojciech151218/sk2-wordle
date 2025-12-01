import http from 'http';
import { URL } from 'url';
import net from 'net';

type WordRequest = {
  method: string;
  word: string;
};

const TCP_HOST = process.env.TCP_HOST ?? '0.0.0.0';
const TCP_PORT = Number(process.env.TCP_PORT ?? 5000);
const HTTP_PORT = Number(process.env.HTTP_PORT ?? 5174);

const log = (...args: unknown[]) => {
  const timestamp = new Date().toISOString();
  console.log(`[proxy ${timestamp}]`, ...args);
};

class TcpConnectionPool {
  private connection: net.Socket | null = null;
  private isConnecting = false;
  private reconnectTimeout: NodeJS.Timeout | null = null;
  private pendingRequest: {
    resolve: (data: string) => void;
    reject: (error: Error) => void;
  } | null = null;

  private createConnection(): Promise<void> {
    return new Promise((resolve, reject) => {
      if (this.connection && !this.connection.destroyed) {
        resolve();
        return;
      }

      this.isConnecting = true;
      const client = net.createConnection(
        { host: TCP_HOST, port: TCP_PORT },
        () => {
          log('TCP connection established');
          this.connection = client;
          this.isConnecting = false;
          this.setupConnectionHandlers();
          resolve();
        },
      );

      client.on('error', (err) => {
        log('TCP connection error', err.message);
        this.isConnecting = false;
        this.connection = null;
        if (this.pendingRequest) {
          this.pendingRequest.reject(err);
          this.pendingRequest = null;
        }
        reject(err);
      });

      client.on('close', () => {
        log('TCP connection closed');
        this.connection = null;
        this.isConnecting = false;
        if (this.pendingRequest) {
          this.pendingRequest.reject(new Error('TCP connection closed'));
          this.pendingRequest = null;
        }
        // Attempt to reconnect after a delay
        if (this.reconnectTimeout) {
          clearTimeout(this.reconnectTimeout);
        }
        this.reconnectTimeout = setTimeout(() => {
          this.ensureConnection().catch(() => {
            // Reconnection failed, will retry on next request
          });
        }, 1000);
      });
    });
  }

  private setupConnectionHandlers(): void {
    if (!this.connection) return;

    let responseBuffer = '';

    this.connection.on('data', (chunk: Buffer) => {
      responseBuffer += chunk.toString();
      
      // Try to parse as complete JSON
      try {
        JSON.parse(responseBuffer);
        // Valid JSON - we have a complete response
        if (this.pendingRequest) {
          const request = this.pendingRequest;
          this.pendingRequest = null;
          const data = responseBuffer;
          responseBuffer = '';
          request.resolve(data);
        }
      } catch {
        // Incomplete JSON, wait for more data
      }
    });
  }

  private async ensureConnection(): Promise<void> {
    if (this.connection && !this.connection.destroyed) {
      return;
    }

    if (this.isConnecting) {
      // Wait for connection to be established
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          reject(new Error('Connection timeout'));
        }, 5000);
        
        const checkInterval = setInterval(() => {
          if (this.connection && !this.connection.destroyed) {
            clearInterval(checkInterval);
            clearTimeout(timeout);
            resolve();
          } else if (!this.isConnecting) {
            clearInterval(checkInterval);
            clearTimeout(timeout);
            if (this.connection && !this.connection.destroyed) {
              resolve();
            } else {
              reject(new Error('Connection failed'));
            }
          }
        }, 100);
      });
    }

    try {
      await this.createConnection();
    } catch (error) {
      log('Failed to establish TCP connection', error);
      throw error;
    }
  }

  async sendRequest(payload: WordRequest): Promise<string> {
    if (this.pendingRequest) {
      throw new Error('Request already in progress');
    }

    await this.ensureConnection();

    if (!this.connection || this.connection.destroyed) {
      throw new Error('TCP connection not available');
    }

    return new Promise((resolve, reject) => {
      const payloadStr = JSON.stringify(payload);
      
      const timeout = setTimeout(() => {
        if (this.pendingRequest) {
          this.pendingRequest.reject(new Error('Request timeout'));
          this.pendingRequest = null;
        }
      }, 30000); // 30 second timeout

      const wrappedResolve = (data: string) => {
        clearTimeout(timeout);
        if (this.pendingRequest) {
          this.pendingRequest = null;
        }
        resolve(data);
      };

      const wrappedReject = (error: Error) => {
        clearTimeout(timeout);
        if (this.pendingRequest) {
          this.pendingRequest = null;
        }
        reject(error);
      };

      this.pendingRequest = { resolve: wrappedResolve, reject: wrappedReject };

      if (!this.connection || this.connection.destroyed) {
        clearTimeout(timeout);
        this.pendingRequest = null;
        reject(new Error('TCP connection not available'));
        return;
      }

      try {
        this.connection.write(payloadStr);
      } catch (error) {
        clearTimeout(timeout);
        this.pendingRequest = null;
        reject(error instanceof Error ? error : new Error('Failed to send request'));
      }
    });
  }
}

const pool = new TcpConnectionPool();

const server = http.createServer(async (req, res) => {
  // CORS headers
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    res.writeHead(200);
    res.end();
    return;
  }

  if (req.method !== 'GET') {
    res.writeHead(405, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: 'Method not allowed' }));
    return;
  }

  try {
    const url = new URL(req.url ?? '/', `http://${req.headers.host}`);
    
    if (url.pathname !== '/api') {
      res.writeHead(404, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ error: 'Not found' }));
      return;
    }

    const method = url.searchParams.get('method');
    const word = url.searchParams.get('word');

    if (!method || !word) {
      res.writeHead(400, { 'Content-Type': 'application/json' });
      res.end(
        JSON.stringify({ error: 'Missing required parameters: method and word' }),
      );
      return;
    }

    const payload: WordRequest = { method, word };
    log('Received request:', payload);

    try {
      const response = await pool.sendRequest(payload);
      log('Sending response:', response);
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end(response);
    } catch (error) {
      const message =
        error instanceof Error ? error.message : 'Unknown TCP error';
      log('TCP error:', message);
      res.writeHead(500, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify({ error: 'TCP connection error', message }));
    }
  } catch (error) {
    const message =
      error instanceof Error ? error.message : 'Unknown error';
    log('Request error:', message);
    res.writeHead(500, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ error: 'Internal server error', message }));
  }
});

server.listen(HTTP_PORT, () => {
  log(`HTTP proxy listening on http://0.0.0.0:${HTTP_PORT}`);
  log(`Proxying TCP traffic to ${TCP_HOST}:${TCP_PORT}`);
  log(`API endpoint: http://0.0.0.0:${HTTP_PORT}/api?method=word&word=hello`);
});

server.on('error', (err) => {
  log('HTTP server error', err.message);
});

