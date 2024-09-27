#include <Wire.h>
#include "Seeed_SHT35.h"
#include "DFRobot_BME280.h"
#include "PMS.h"
#include "MHZ19.h"

#define SEA_LEVEL_PRESSURE 1013.0f

// Global variables for sensor data
float temp, hum, pressure, alt, pm1, pm25, pm10, CO2;
float tempAvg, humAvg, pressureAvg, altAvg, pm1Avg, pm25Avg, pm10Avg, CO2Avg;
String str = "";
String id = "0";

// Sensor objects
DFRobot_BME280_IIC bme(&Wire, 0x77);
PMS pms(Serial2);
PMS::DATA data;
MHZ19 myMHZ19;
unsigned long getDataTimer = 0;
SHT35 sht35(21);  // Assuming SCL is connected to pin 21

void setup() {
    Serial.begin(9600);

    // Initialize SHT35
    sht35.init();

    // Initialize MH-Z19B
    Serial1.begin(9600);  // MH-Z19B Serial
    myMHZ19.begin(Serial1);  // Pass Serial to library
    myMHZ19.autoCalibration(false);  // Disable auto calibration

    // Initialize PMS5003
    Serial2.begin(9600);  // PMS5003 Serial
    pms.passiveMode();  // Switch to passive mode
    pms.wakeUp();

    // Initialize BME280
    while (bme.begin() != DFRobot_BME280_IIC::eStatusOK) {
        Serial.println("BME280 initialization failed");
        delay(2000);
    }

    // Initialize Serial3
    Serial3.begin(115200);
}

void loop() {
    // Static variables to store accumulated values and counts
    static float tempSum = 0, humSum = 0, pressureSum = 0, altSum = 0;
    static float pm1Sum = 0, pm25Sum = 0, pm10Sum = 0, CO2Sum = 0;
    static int sampleCount = 0;
    static unsigned long startTime = millis();  // Start time for 5-second interval

    readSensors();  // Read sensor data

    // Accumulate values
    tempSum += temp;
    humSum += hum;
    pressureSum += pressure;
    altSum += alt;
    pm1Sum += pm1;
    pm25Sum += pm25;
    pm10Sum += pm10;
    CO2Sum += CO2;  // Fixed accumulation of CO2

    sampleCount++;

    // Check if 5 seconds have passed
    if (millis() - startTime >= 9500) {
        // Calculate averages
        tempAvg = tempSum / sampleCount;
        humAvg = humSum / sampleCount;
        pressureAvg = pressureSum / sampleCount;
        altAvg = altSum / sampleCount;
        pm1Avg = pm1Sum / sampleCount;
        pm25Avg = pm25Sum / sampleCount;
        pm10Avg = pm10Sum / sampleCount;
        CO2Avg = CO2Sum / sampleCount;  // Calculate CO2 average

        // Reset accumulators and counter
        tempSum = humSum = pressureSum = altSum = 0;
        pm1Sum = pm25Sum = pm10Sum = CO2Sum = 0;
        sampleCount = 0;
        startTime = millis();  // Reset the timer

        sendData();  // Send averaged data
    }
}

void readSensors() {
    // Read SHT35
    sht35.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, &temp, &hum);
    pressure = bme.getPressure();
    alt = bme.calAltitude(SEA_LEVEL_PRESSURE, pressure) / 100;

    // Read PM sensor data
    pms.requestRead();
    if (pms.readUntil(data)) {
        pm1 = data.PM_AE_UG_1_0;
        pm25 = data.PM_AE_UG_2_5;
        pm10 = data.PM_AE_UG_10_0;
    }

    CO2 = myMHZ19.getCO2();  // Request CO2 (as ppm)
}

void sendData() {
    // Send averaged data over Serial3
    str = "{\"id\":" + String(id) +
          ",\"air_temperature\":" + String(tempAvg) +
          ",\"humidity\":" + String(humAvg) +
          ",\"pressure\":" + String(pressureAvg) +
          ",\"pm1\":" + String(pm1Avg) +
          ",\"pm2_5\":" + String(pm25Avg) +
          ",\"pm10\":" + String(pm10Avg) +
          ",\"co2\":" + String(CO2Avg) + "}";  // Correctly close the JSON string
    
    Serial3.println(str);
    
    Serial.println("------xxxxxxxxxxxxxxxxxxxxxxxx--------");

    Serial.println("Temp: " + String(tempAvg));
    Serial.println("Humi: " + String(humAvg));
    Serial.println("Air Pressure: " + String(pressureAvg));
    Serial.println("Altitude: " + String(alt));
    Serial.println("PM1.0: " + String(pm1Avg));
    Serial.println("PM2.5: " + String(pm25Avg));
    Serial.println("PM10.0: " + String(pm10Avg));
    Serial.println("CO2: " + String(CO2Avg));
}
