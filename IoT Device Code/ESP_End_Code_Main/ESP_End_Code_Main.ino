#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";
WiFiClient espClient;

// MQTT broker settings
const char* mqtt_server = "103.237.39.27";
const char* mqtt_topic = "esp8266/sensorData";  // Topic to publish data to
PubSubClient mqttClient(espClient);

// Data
String incomingData; // Use String class to store incoming serial data

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    mqttClient.setServer(mqtt_server, 1883);  // Set the MQTT broker and port
}

void loop() {
    if (!mqttClient.connected()) {
        while (!mqttClient.connected()) {
            mqttClient.connect("ESP8266Client");
        }
    } 
    mqttClient.loop(); // Required to keep MQTT Connection
    delay(10);

    // Read Serial in chunks to avoid missing data
    while (Serial.available() > 0) {
        char receivedChar = Serial.read(); // Read one character at a time
        incomingData += receivedChar;      // Append to the incomingData string

        // Check for newline (end of data)
        if (receivedChar == '\n') {
            if (mqttClient.connected()) {
                mqttClient.publish(mqtt_topic, incomingData.c_str(),true,1);
            }
            incomingData = ""; // Clear the buffer for the next message
        }
    }
}

