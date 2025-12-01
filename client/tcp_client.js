const net = require('net');

const client = new net.Socket();

const request = {
  method: 'word',
  word: 'hello'
};

client.connect(5000, '0.0.0.0', () => {
  console.log('Connected to server');
  client.write(JSON.stringify(request));
});

client.on('data', (data) => {
  console.log('Received:', data.toString());
});