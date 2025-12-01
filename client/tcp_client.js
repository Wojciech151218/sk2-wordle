const net = require('net');

const client = new net.Socket();

client.connect(5000, '0.0.0.0', () => {
  console.log('Connected to server');
  client.write('Hello from Node TCP client');
});

// client.on('data', (data) => {
//   console.log('Received:', data.toString());
// });