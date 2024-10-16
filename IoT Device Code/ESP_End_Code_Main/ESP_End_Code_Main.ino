#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi credentials
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";
const char* serverUrl = "http://103.237.39.27:1880/cfd1";

WiFiClient espClient;
HTTPClient http;

// Data
String incomingData = ""; 


void setup() {
    Serial.begin(115200);
    setup_wifi();
}


void loop() {
    // Check WiFi connection and reconnect if needed
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi(); 
    }

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
  http.begin(espClient,serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.POST(incomingData);
  http.end();
}

void setup_wifi() {
    delay(500);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

