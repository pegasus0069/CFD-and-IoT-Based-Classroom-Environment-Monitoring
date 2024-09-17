#include <WiFi.h>
#include <PubSubClient.h>

// Sensor data (dummy data)
float temp = 0;
float hum = 0;
float Pressure = 0;
float alt = 0;
float PM1 = 0;
float PM25 = 0;
float PM10 = 0;
float CO2 = 0;
int id = 99;

const char* ssid = "Mahir 2.4GHz";
const char* password = "01741238814";
const char* mqtt_server = "103.237.39.27"; 

WiFiClient espClient;
PubSubClient client(espClient);
int i = 0;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  //String payload = "{\"temperature\":25, \"humidity\":60}";
  //client.publish("sensor/data", String(i).c_str());
  sendData(id, temp, hum, Pressure, alt, PM1, PM25, PM10, CO2);
  i++;

  delay(2000);
}

void setup_wifi() {
  delay(10);
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
    if (client.connect("ESP32Client")) {
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
void sendData(int id, float tem, float hum, float pressure, float alt, float pm1, float pm25, float pm10, float co2) {
  // Create a JSON-like string payload
  String payload = "{\"id\":" + String(id) + ",\"air_temperature\":" + String(tem) + ",\"humidity\":" + String(hum) +
                   ",\"pressure\":" + String(pressure) + ",\"pm1\":" + String(pm1) + ",\"pm2_5\":" + String(pm25) + 
                   ",\"pm10\":" + String(pm10) + ",\"co2\":" + String(co2) + "}";
  
  // Publish the payload to the topic
  client.publish("esp8266/sensorData", payload.c_str());
  
  Serial.print("Published data: ");
  Serial.println(payload);
}