const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const mqtt = require('mqtt');
const path = require('path');
const bodyParser = require('body-parser');
const fs = require('fs');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const brokerUrl = 'mqtt://broker.mqtt-dashboard.com';
const client = mqtt.connect(brokerUrl);
const profilesFilePath = path.join(__dirname, 'profiles.json');

// Load profiles from file
const loadProfiles = () => {
    try {
        if (fs.existsSync(profilesFilePath)) {
            const data = fs.readFileSync(profilesFilePath, 'utf8');
            return JSON.parse(data);
        }
    } catch (err) {
        console.error('Error loading profiles:', err);
    }
    return [];
};

// Save profiles to file
const saveProfiles = () => {
    try {
        fs.writeFileSync(profilesFilePath, JSON.stringify(profiles, null, 2), 'utf8');
    } catch (err) {
        console.error('Error saving profiles:', err);
    }
};

let profiles = loadProfiles(); // Initialize profiles from file
let sensorData = {
    temp: null,
    ph: null,
    turbidity: null,
    tds: null,
};

let currentProfileId = null; // Track currently selected profile

client.on('connect', () => {
    console.log('Connected to MQTT broker');
});

//update data
client.on('message', (topic, message) => {
    const value = message.toString();
    if (currentProfileId) {
        const currentProfile = profiles.find(profile => profile.id === currentProfileId);
        if (currentProfile && currentProfile.sensors) {
            //ekstraksi nama topik
            const currTemp ="kualair/"+currentProfile.sensors.TEMP+"/TEMP";
            const currPh ="kualair/"+currentProfile.sensors.PH+"/PH";
            const currTur ="kualair/"+currentProfile.sensors.TUR+"/TURBIDITY";
            const currTds ="kualair/"+currentProfile.sensors.TDS+"/TDS";
            const currTopics = {currTemp, currPh, currTur, currTds};
            //update data sensor
            if (topic ===currTopics.currTemp) sensorData.temp = value;
            else if (topic === currTopics.currPh) sensorData.ph = value;
            else if (topic === currTopics.currTur) sensorData.turbidity = value;
            else if (topic === currTopics.currTds) sensorData.tds = value;
        }
    }

    wss.clients.forEach(ws => {
        if (ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(sensorData));
        }
    });
});

// Middleware
app.use(bodyParser.json());

// Default routing to profiles.html
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'profiles.html'));
});

// Static files middleware
app.use(express.static(path.join(__dirname, 'public')));

// Add a new profile
app.post('/api/profiles', (req, res) => {
    const { name } = req.body;
    if (name) {
        const newProfile = {
            id: profiles.length + 1,
            name,
            sensors: null,
        };
        profiles.push(newProfile);
        saveProfiles(); // Save profiles to file
        res.status(201).json({ message: 'Profile added', profiles });
    } else {
        res.status(400).json({ message: 'Name is required' });
    }
});

// Get all profiles
app.get('/api/profiles', (req, res) => {
    res.status(200).json(profiles);
});

// Delete a profile
app.delete('/api/profiles/:id', (req, res) => {
    const { id } = req.params;
    const index = profiles.findIndex(profile => profile.id === parseInt(id));
    if (index !== -1) {
        profiles.splice(index, 1);
        saveProfiles(); // Save profiles to file
        res.status(200).json({ message: 'Profile deleted', profiles });
    } else {
        res.status(404).json({ message: 'Profile not found' });
    }
});

// Configure sensors for a profile
app.post('/api/profiles/:id/configure', (req, res) => {
    const { id } = req.params;
    const { PH, TEMP, TUR, TDS } = req.body;
    const profile = profiles.find(profile => profile.id === parseInt(id));

    if (profile) {
        profile.sensors = { PH, TEMP, TUR, TDS };
        saveProfiles(); // Save profiles to file
        res.status(200).json({ message: 'Sensors configured', profile });   
    } else {
        res.status(404).json({ message: 'Profile not found' });
    }
});

// Select active profile
app.post('/deviceSelection', (req, res) => {
    const { profileId } = req.body;
    const profile = profiles.find(p => p.id === parseInt(profileId));

    if (profile) {
        currentProfileId = parseInt(profileId); // Save active profile ID
        // console.log(profile.sensors);
        const { PH, TEMP, TUR, TDS }  = profile.sensors;
        const topics = ["kualair/"+PH+"/PH", "kualair/"+TEMP+"/TEMP", "kualair/"+TUR+"/TURBIDITY", "kualair/"+TDS+"/TDS"];
        client.unsubscribe(["kualair/+/PH", "kualair/+/TEMP", "kualair/+/TURBIDITY", "kualair/+/TDS"]);
        client.subscribe(topics, err => {
            if (!err) {
                console.log(`Subscribed to topics for profile ${profile.name}: ${topics.join(', ')}`);
                saveProfiles(); // Save profiles to file
                res.json({ success: true, message: `Profile ${profile.name} selected` });
            } else {
                res.status(500).json({ success: false, message: 'Failed to subscribe to topics' });
            }
        });
    } else {
        res.status(404).json({ success: false, message: 'Profile not found' });
    }
});

// Get active profile
app.get('/getActiveProfile', (req, res) => {
    const profile = profiles.find(p => p.id === currentProfileId);
    if (profile) {
        res.json({ success: true, profile });
    } else {
        res.json({ success: false });
    }
});

server.listen(3000, () => {
    console.log('Server running on http://localhost:3000');
});
