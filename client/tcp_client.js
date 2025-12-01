const net = require('net');

const client = new net.Socket();

// Only one request to send
const request = { method: 'word', word: 'hello' };

let running = true;

client.connect(5000, '0.0.0.0', () => {
  console.log('Connected to server');
  // Send the single request
  if (running) {
    console.log('Sending request:', request);
    client.write(JSON.stringify(request));
  }
});

client.on('data', (data) => {
  console.log('Received:', data.toString());
  // Close after receiving the response to the only request
  running = false;
  client.end();
});

client.on('close', () => {
  console.log('Connection closed');
});

client.on('error', (err) => {
  console.error('Error:', err);
});

// Handle Ctrl+C to stop the client gracefully
process.on('SIGINT', () => {
  console.log('\nStopping client...');
  running = false;
  client.end();
  process.exit(0);
});