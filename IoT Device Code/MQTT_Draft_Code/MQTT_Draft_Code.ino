#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "INSERT_YOUR_SSID";
const char* password = "INSERT_YOUR_PASSWORD";
const char* mqtt_server = "INSERT_YOUR_MQTT_BROKER_IP"; 

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
  client.publish("esp32/data", String(i).c_str());
  i++;

  delay(5000);
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