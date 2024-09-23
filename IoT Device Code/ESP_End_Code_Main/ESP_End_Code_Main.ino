#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerialLite.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";

// MQTT broker settings
const char* mqtt_server = "103.237.39.27"; 
const char* mqtt_topic = "esp8266/sensorData";  // Topic to publish data to

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);

// Sensor data (dummy data)
int id = 1;
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

void recvMsg(uint8_t *data, size_t len) {
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  WebSerial.begin(&server);
  server.begin();

  client.setServer(mqtt_server, 1883);  // Set the MQTT broker and port
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  WebSerial.println("New loop");
  client.loop();

  // Check for serial input
  
  readSerialData();
  
  
  //delay(5000);  // Delay for testing, adjust as needed
}

void setup_wifi() {
  delay(500);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      // Successfully connected
    } else {
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
  WebSerial.print("sendData function called");
  // Create a JSON-like string payload
  String payload = "{\"id\":" + String(id) + ",\"air_temperature\":" + String(tem) + ",\"humidity\":" + String(hum) +
                   ",\"pressure\":" + String(pressure) + ",\"pm1\":" + String(pm1) + ",\"pm2_5\":" + String(pm25) + 
                   ",\"pm10\":" + String(pm10) + ",\"co2\":" + String(co2) + "}";

  // Publish the payload to the topic
  WebSerial.println(payload.c_str());
  if (client.publish(mqtt_topic, payload.c_str())) {
    // Publish successful
    WebSerial.println("Publish successful");
  }
}

// Function to read serial data and handle timeout
void readSerialData() {
  // Check if there's serial data available
  if (Serial.available()) {
    WebSerial.print("Receiving data from Arduino");
    lastSerialReadTime = millis();  // Reset the timeout timer

    while (Serial.available()) {
      char incomingChar = Serial.read();  // Read each character
      if (incomingChar == '\n') {  // Newline indicates end of message
        parseDataString(msg);  // Parse the complete message
        sendData(id, temp, hum, Pressure, alt, PM1, PM25, PM10, CO2);  // Send parsed data via MQTT
        msg = "";  // Clear the message buffer
      } else {
        msg += incomingChar;  // Append the incoming character to the message
      }
    }
  }

  // Check if no data has been received for the timeout period
  if (millis() - lastSerialReadTime > serialTimeout && msg.length() > 0) {
    msg = "";  // Reset the buffer if incomplete data has been sitting for too long
  }
}

// This function is called when data is received from the browser
void onWebSerialReceive(uint8_t *data, size_t len) {
  String receivedText = "";
  for (int i = 0; i < len; i++) {
    receivedText += char(data[i]);
  }
  Serial.println("Received via WebSerial: " + receivedText);
  
  // Echo back the received message to the browser
  WebSerial.print("Echo: ");
  WebSerial.println(receivedText);
}