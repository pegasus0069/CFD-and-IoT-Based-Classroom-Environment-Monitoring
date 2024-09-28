#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";

// MQTT broker settings
const char* mqtt_server = "103.237.39.27";
const char* mqtt_topic = "esp8266/sensorData";  

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Data
String incomingData = ""; 
String timestamp = "";  

void setup() {
    Serial.begin(115200);
    setup_wifi();
    mqttClient.setServer(mqtt_server, 1883);  // Set the MQTT broker and port
}

void loop() {
    if (!mqttClient.connected()) {
        reconnectMQTT();
    } 
    mqttClient.loop(); // Required to stay connected
    delay(10);

    // Read Serial
    while (Serial.available() > 0) {
        char receivedChar = Serial.read();
        if (receivedChar == '\n') {  
            // Trim whitespace and send data
            incomingData.trim(); 
            updateTime();
            sendData();
            incomingData = "";  // Clear the buffer after processing
        } else {
            incomingData += receivedChar;  // Accumulate incoming data
        }
    }
}

void sendData() {
    if (mqttClient.connected() && !incomingData.isEmpty()) { // Check if connected and data is not empty
        mqttClient.publish(mqtt_topic, incomingData.c_str());
    }
}

void setup_wifi() {
    delay(500);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client")) {
            Serial.println("Connected to MQTT broker.");
        } else {
            Serial.print("Failed to connect to MQTT broker. Retrying in 5 seconds...");
            delay(5000);  // Wait 5 seconds before retrying
        }
    }
}
// Function to format timestamp for MySQL
updateTime() {
    timeClient.update();  // Update the time from NTP
    unsigned long epochTime = timeClient.getEpochTime();  // Get epoch time
    // Convert epoch time to time structure
    struct tm *timeinfo = localtime(&epochTime);
    // Create a formatted timestamp string
    char buffer[20];  // YYYY-MM-DD HH:MM:SS (19 chars + null terminator)
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", 
            timeinfo->tm_year + 1900,  // Year since 1900
            timeinfo->tm_mon + 1,      // Month [0-11]
            timeinfo->tm_mday,         // Day of the month
            timeinfo->tm_hour,         // Hour [0-23]
            timeinfo->tm_min,          // Minutes [0-59]
            timeinfo->tm_sec);         // Seconds [0-59]
    timestamp = String(buffer);  // Return as a String
}
