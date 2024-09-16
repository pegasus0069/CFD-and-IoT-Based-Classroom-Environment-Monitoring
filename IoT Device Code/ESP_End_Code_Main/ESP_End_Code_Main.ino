#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "Fab@IUB";
const char* password = "@makers#";

// MQTT broker settings
const char* mqtt_server = "INSERT_YOUR_MQTT_BROKER_IP"; 

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor data (dummy data)
float temp = 0;
float hum = 0;
float Pressure = 0;
float alt = 0;
float PM1 = 0;
float PM25 = 0;
float PM10 = 0;
float CO2 = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // Set the MQTT broker and port
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(Serial.available()){
    String msg = "";
    while(Serial.available()){
      msg += char(Serial.read());
      delay(50);
    }
    parseDataString(msg);
    sendData(temp, hum, Pressure, alt, PM1, PM25, PM10, CO2);
  }

// delay(5000); // Publish every 5 seconds
}

void setup_wifi() {
  delay(500);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {  // Client ID
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" - ");
      switch (client.state()) {
        case -4: Serial.println("MQTT connect failed: Server unavailable"); break;
        case -3: Serial.println("MQTT connect failed: Connection refused"); break;
        case -2: Serial.println("MQTT connect failed: Network error"); break;
        case -1: Serial.println("MQTT connect failed: Protocol error"); break;
        default: Serial.println("MQTT connect failed: Unknown error"); break;
      }
      delay(5000);
    }
  }
}

void parseDataString(String inputString) {
  char inputChars[inputString.length() + 1];
  inputString.toCharArray(inputChars, inputString.length() + 1);
  char *token;

  token = strtok(inputChars, ";");
  float temperature = atof(token);

  token = strtok(NULL, ";");
  float humidity = atof(token);

  token = strtok(NULL, ";");
  float pressure = atof(token);

  token = strtok(NULL, ";");
  float altitude = atof(token);

  token = strtok(NULL, ";");
  float pm1 = atof(token);

  token = strtok(NULL, ";");
  float pm25 = atof(token);

  token = strtok(NULL, ";");
  float pm10 = atof(token);

  token = strtok(NULL, ";");
  float co2 = atof(token);

  temp = temperature; 
  hum = humidity;
  Pressure = pressure;
  alt = altitude;
  PM1 = pm1;
  PM25 = pm25;
  PM10 = pm10;
  CO2 = co2;
}

void sendData(float tem, float hum, float pressure, float alt, float pm1, float pm25, float pm10, float co2) {
  // Create a JSON-like string payload
  String payload = "{\"air_temperature\":" + String(tem) + ",\"humidity\":" + String(hum) +
                   ",\"pressure\":" + String(pressure) + ",\"pm1\":" + String(pm1) + ",\"pm25\":" + String(pm25) + 
                   ",\"pm10\":" + String(pm10) + ",\"co2\":" + String(co2) + "}";
  
  // Publish the payload to the topic
  client.publish("esp8266/sensorData", payload.c_str());
  
  Serial.print("Published data: ");
  Serial.println(payload);
}