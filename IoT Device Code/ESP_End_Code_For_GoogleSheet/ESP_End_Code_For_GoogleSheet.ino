#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "Fab@IUB";
const char* password = "@makers#";

//----------------------------------------Host & httpsPort
const char* host = "script.google.com";
const int httpsPort = 443;
//----------------------------------------

WiFiClientSecure client; 

String GAS_ID = "AKfycbxQPOYiSTgKx5GBCACA2hyqzPyDCTGeC7530s772562OATRdThLpDMbpVqTaYoTQWf3"; //--> spreadsheet script ID


//dummy data
float temp = 0;
float hum = 0;
float Pressure = 0;
float alt = 0;
float PM1 = 0;
float PM25 = 0;
float PM10 = 0;
float CO2 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(500);
  
  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
    //----------------------------------------
  }
  //----------------------------------------
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //----------------------------------------

  client.setInsecure();
}

void loop() {
 if(Serial.available()){
    String msg = "";
    while(Serial.available()){
      msg += char(Serial.read());
      delay(50);
    }
  parseDataString(msg);
  sendData(temp, hum, Pressure, alt, PM1, PM25, PM10, CO2); //--> Calls the sendData Subroutine
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
  alt= altitude;
  PM1= pm1;
  PM25= pm25;
  PM10= pm10;
  CO2= co2;
                        
}

// Subroutine for sending data to Google Sheets
void sendData(float tem, float hum, float pressure, float alt, float pm1, float pm25, float pm10, float co2) {
  Serial.println("==========");
  Serial.print("connecting to ");
  Serial.println(host);
  
  //----------------------------------------Connect to Google host
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  //----------------------------------------

  //----------------------------------------Processing data and sending data
  String string_temperature =  String(temp);
  String string_humidity =  String(hum); 
  String string_pressure =  String(Pressure);
  String string_alt =  String(alt);
  String string_pm1 =  String(PM1);
  String string_pm25 =  String(PM25);
  String string_pm10 =  String(PM10);
  String string_co2 =  String(CO2);
 
  String url = "/macros/s/" + GAS_ID + "/exec?temperature=" + string_temperature + "&humidity=" + string_humidity + "&pressure=" + string_pressure + "&alt=" + string_alt+ "&pm1=" + string_pm1+ "&pm25=" + string_pm25+ "&pm10=" + string_pm10+ "&CO2=" + string_co2 ;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //----------------------------------------

  //----------------------------------------Checking whether the data was sent successfully or not
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.print("reply was : ");
  Serial.println(line);
  Serial.println("closing connection");
  Serial.println("==========");
  Serial.println();
  //----------------------------------------
} 
//==============================================================================
