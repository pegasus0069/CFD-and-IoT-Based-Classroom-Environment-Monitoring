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
char timestamp[20]; // Buffer to store the formatted timestamp
char incomingData[100]; // Buffer to store incoming serial data (adjust size as needed)

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
        Serial.readBytesUntil('\n', incomingData, sizeof(incomingData)); // Read incoming data into the buffer
    }

    // Send data to server
    if (mqttClient.connected()) {
        mqttClient.publish(mqtt_topic, incomingData);
    }
}

void updateTime() {
    timeClient.update();
    time_t rawTime = timeClient.getEpochTime();
    struct tm* timeinfo;
    timeinfo = localtime(&rawTime);
    
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
}
