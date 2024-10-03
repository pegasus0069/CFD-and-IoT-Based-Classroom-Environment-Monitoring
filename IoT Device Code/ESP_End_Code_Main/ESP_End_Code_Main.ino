#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "CFDIUB";
const char* password = "#cfd@iub#";

// MQTT broker settings
const char* mqtt_server = "103.237.39.27";
const char* mqtt_topic = "esp8266/cfd1";  

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
    // Check WiFi connection and reconnect if needed
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi(); 
    }

    if (!mqttClient.connected()) {
        reconnectMQTT();
    } 
    mqttClient.loop(); // Required to stay connected
    delay(10);

    // Read Serial
    while (Serial.available() > 0) {
        char receivedChar = Serial.read();
        if (receivedChar == '\n') {  
            // Trim whitespace and update time
            incomingData.trim(); 
            sendData();  
            incomingData = ""; 
        } else {
            incomingData += receivedChar; 
        }
    }
}

void sendData() {
    if (mqttClient.connected() && !incomingData.isEmpty()) { // Check if connected and data is not empty
        
        mqttClient.publish(mqtt_topic,incomingData.c_str()); 
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
        } else {
            delay(2000);  
        }
    }
}

