const net = require('net');

const client = new net.Socket();

// Array of requests to send (will cycle through these)
const requests = [
  { method: 'word', word: 'hello' },
  { method: 'word', word: 'world' },
  { method: 'word', word: 'test' }
];

let requestIndex = 0;
let running = true;

client.connect(5000, '0.0.0.0', () => {
  console.log('Connected to server');
  // Send the first request
  sendNextRequest();
});

function sendNextRequest() {
  if (!running) return;
  
  // Cycle through requests array
  const request = requests[requestIndex % requests.length];
  console.log(`Sending request ${requestIndex + 1}:`, request);
  client.write(JSON.stringify(request));
  requestIndex++;
}

client.on('data', (data) => {
  console.log('Received:', data.toString());
  // Wait a bit before sending the next request
  // This gives time for the server to process
  setTimeout(() => {
    sendNextRequest();
  }, 100);
});

client.on('close', () => {
  console.log('Connection closed');
});

client.on('error', (err) => {
  console.error('Error:', err);
});

// Handle Ctrl+C to stop the loop gracefully
process.on('SIGINT', () => {
  console.log('\nStopping client...');
  running = false;
  client.end();
  process.exit(0);
});