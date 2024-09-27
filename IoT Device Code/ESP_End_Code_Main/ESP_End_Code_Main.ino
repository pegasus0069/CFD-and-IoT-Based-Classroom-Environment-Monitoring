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

    // Read Serial
    if (Serial.available() > 0) {
        incomingData = Serial.readStringUntil('\n'); 
        if (mqttClient.connected()) {
            mqttClient.publish(mqtt_topic, incomingData.c_str()); 
        }
    }
}

