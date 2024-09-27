#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 6 * 3600, 60000); // UTC offset 0

// WiFi
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";
WiFiClient espClient;

// MQTT broker settings
const char* mqtt_server = "103.237.39.27";
const char* mqtt_topic = "esp8266/sensorData";  // Topic to publish data to
PubSubClient mqttClient(espClient);

// Data
String timestamp; // Change to String to store the formatted timestamp
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
    updateTime();

    // Read Serial
    if (Serial.available() > 0) {
        incomingData = Serial.readStringUntil('\n'); // Read incoming data as a String
    }

    // Send data to server
    if (mqttClient.connected()) {
        mqttClient.publish(mqtt_topic, incomingData.c_str()); // Publish the string data
    }
}

void updateTime() {
    timeClient.update();
    time_t rawTime = timeClient.getEpochTime();
    struct tm* timeinfo;
    timeinfo = localtime(&rawTime);
    
    // Format timestamp and assign to String
    char buffer[20]; // Buffer to format timestamp
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    timestamp = String(buffer);
}
