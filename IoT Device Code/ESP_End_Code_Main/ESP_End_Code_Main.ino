#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>  // Include the OTA library

// WiFi credentials
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";

// MQTT broker settings
const char* mqtt_server = "103.237.39.27";
const char* mqtt_topic = "esp8266/sensorData";  // Topic to publish data to

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor data (dummy data)
int id = 0;
float temp = 0;
float hum = 0;
float Pressure = 0;
float alt = 0;
float PM1 = 0;
float PM25 = 0;
float PM10 = 0;
float CO2 = 0;

String msg = "";  // Store incoming serial data
unsigned long lastSerialReadTime = 0;  // Track the time for serial reading
const unsigned long serialTimeout = 1500;  // 1500 ms timeout for receiving complete data

void setup() {
    Serial.begin(115200);
    setup_wifi();

    client.setServer(mqtt_server, 1883);  // Set the MQTT broker and port

    // Initialize OTA
    ArduinoOTA.setPassword("espota");
    setupOTA();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    } else {
        Serial.println("Connected to MQTT broker.");
    }
    client.loop();

    // Handle OTA updates
    ArduinoOTA.handle();

    // Check for serial input
    readSerialData();
}

void setup_wifi() {
    delay(500);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.print("Successfully connected to: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
}

void setupOTA() {
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {  // U_SPIFFS
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
    Serial.println("OTA ready");
}

void reconnect() {
    while (!client.connected()) {
        if (client.connect("ESP8266Client")) {
            Serial.println("Connected to MQTT broker.");
        } else {
            Serial.print("Failed to connect to MQTT broker. Retrying in 5 seconds...");
            delay(5000);  // Wait 5 seconds before retrying
        }
    }
}

// Parse incoming serial data
void parseDataString(String inputString) {
    char inputChars[inputString.length() + 1];
    inputString.toCharArray(inputChars, inputString.length() + 1);
    char* token;

    token = strtok(inputChars, ";");
    if (token != NULL) temp = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) hum = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) Pressure = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) alt = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) PM1 = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) PM25 = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) PM10 = atof(token);

    token = strtok(NULL, ";");
    if (token != NULL) CO2 = atof(token);
}

// Publish data to MQTT broker
void sendData(int id, float tem, float hum, float pressure, float alt, float pm1, float pm25, float pm10, float co2) {
    // Create a JSON-like string payload
    String payload = "{\"id\":" + String(id) +
                     ",\"air_temperature\":" + String(tem) +
                     ",\"humidity\":" + String(hum) +
                     ",\"pressure\":" + String(pressure) +
                     ",\"pm1\":" + String(pm1) +
                     ",\"pm2_5\":" + String(pm25) +
                     ",\"pm10\":" + String(pm10) +
                     ",\"co2\":" + String(co2) + "}";

    // Publish the payload to the topic
    if (client.publish(mqtt_topic, payload.c_str())) {
        Serial.println("Data Published:");
        Serial.println(payload);
    } else {
        Serial.println("Data Publish Failed.");
    }
}

// Function to read serial data and handle timeout
void readSerialData() {
    // Check if there's serial data available
    if (Serial.available()) {
        lastSerialReadTime = millis();  // Reset the timeout timer

        while (Serial.available()) {
            char incomingChar = Serial.read();  // Read each character
            if (incomingChar == '\n') {  // Newline indicates end of message
                parseDataString(msg);  // Parse the complete message
                sendData(id, temp, hum, Pressure, alt, PM1, PM25, PM10, CO2);  // Send parsed data via MQTT
                Serial.println("Device Data:");
                Serial.println(msg);
                msg = "";  // Clear the message buffer
            } else {
                msg += incomingChar;  // Append the incoming character to the message
            }
        }
    }

    // Check if no data has been received for the timeout period
    if (millis() - lastSerialReadTime > serialTimeout && msg.length() > 0) {
        Serial.println("Incomplete data received. Clearing buffer.");
        msg = "";  // Reset the buffer if incomplete data has been sitting for too long
    }
}
