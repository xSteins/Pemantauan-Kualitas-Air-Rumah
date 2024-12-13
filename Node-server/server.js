const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const mqtt = require('mqtt');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const brokerUrl = 'mqtt://broker.mqtt-dashboard.com';
const topics = ['RALVIN01TEMP', 'RALVIN01PH', 'RALVIN01TURBIDITY', 'RALVIN01TDS'];

const client = mqtt.connect(brokerUrl);

let sensorData = {
    temp: null,
    ph: null,
    turbidity: null,
    tds: null,
};

client.on('connect', () => {
    console.log('Connected to MQTT broker');
    client.subscribe(topics, (err) => {
        if (!err) {
            console.log('Subscribed to topics');
        }
    });
});

// todo : karena asumsi di esp bisa kirim data bentuk selain string
// mungkin bisa direvisi disini supaya langsung subscribe satu topik
client.on('message', (topic, message) => {
    const value = message.toString();
    switch (topic) {
        case 'RALVIN01TEMP':
            sensorData.temp = value;
            break;
        case 'RALVIN01PH':
            sensorData.ph = value;
            break;
        case 'RALVIN01TURBIDITY':
            sensorData.turbidity = value;
            break;
        case 'RALVIN01TDS':
            sensorData.tds = value;
            break;
    }

    // broadcast datanya ke wss
    wss.clients.forEach((ws) => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(sensorData));
        }
    });
});

wss.on('connection', (ws) => {
    console.log('WebSocket client connected');
    ws.send(JSON.stringify(sensorData));// kirim datanya berbentuk json
});

app.use(express.static('public')); // routing halaman ke folder public

server.listen(3000, () => {
    console.log('Server running on http://localhost:3000');
});
