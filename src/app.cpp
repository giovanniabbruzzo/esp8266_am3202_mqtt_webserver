#include "app.h"
#include <ESP8266WiFi.h>
#include "common_definitions.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

DHT dht(DHTPIN, DHTTYPE);

app_t app;

void app_init(void){
    // app.flags.val = 0;
    app.sensors.hum = 0;
    app.sensors.temp = 0;
    app.ipAddr = WiFi.localIP().toString();
    app.macAddr = WiFi.macAddress();
    app.fwVersion = String(FW_VERSION_MAJOR)+"."+String(FW_VERSION_MINOR)+"."+String(FW_VERSION_PATCH);

    dht.begin();
}

void app_get_sensor_data(void){
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      PRINTLN("Failed to read from DHT sensor!");
    }
    else {
      app.sensors.temp = newT;
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      PRINTLN("Failed to read from DHT sensor!");
    }
    else {
      app.sensors.hum = newH;
    }
}