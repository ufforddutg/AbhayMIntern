const WebSocket = require('ws');
const http = require('http');
const express = require('express');

const app = express();
const server = http.createServer(app);

const wss = new WebSocket.Server({ server });

let esp32Socket = null;
let clientSocket = null;

wss.on('connection', ws => {
  console.log('Client connected');

  // Determine which connection is ESP32 and which is the client
  ws.on('message', message => {
    console.log('Received message:', message);

    if (message === 'ESP32') {
      esp32Socket = ws;
      console.log('ESP32 connected');
    } else if (message === 'Client') {
      clientSocket = ws;
      console.log('Client connected');
    } else {
      if (esp32Socket && esp32Socket.readyState === WebSocket.OPEN) {
        esp32Socket.send(message);
        console.log('Message sent to ESP32:', message);
      }
      
      if (clientSocket && clientSocket.readyState === WebSocket.OPEN) {
        clientSocket.send(message);
        console.log('Message sent to client:', message);
      }
    }
  });

  ws.on('close', () => {
    console.log('Client disconnected');
    if (ws === esp32Socket) {
      esp32Socket = null;
      console.log('ESP32 disconnected');
    } else if (ws === clientSocket) {
      clientSocket = null;
      console.log('Client disconnected');
    }
  });

  ws.on('error', error => {
    console.error('WebSocket error:', error);
  });
});

const port = 8080;
server.listen(port, () => {
  console.log(`WebSocket server running on ws://192.168.137.77:${port}`);
});
