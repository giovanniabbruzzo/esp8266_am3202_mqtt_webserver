// Import required libraries
#include "common_definitions.h"
#include <Hash.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
/*
  sectrets.h will contain at least:
  extern const char* ssid;
  extern const char* password;
  extern const char* mqtt_server;
  extern const int mqtt_port;
  extern const char* mqtt_user;
  extern const char* mqtt_password;
  extern const char* mqtt_topic;
  extern const char* ota_psk;
  And sectrets.cpp will define them
*/
#include "webserver.h"
#include "mqtt_hal.h"
#include <ArduinoOTA.h>

extern const char* ssid;
extern const char* password;

#define DHTPIN D2     // Digital pin connected to the DHT sensor

#define DHTTYPE    DHT22     // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
volatile float t = 0.0;
volatile float h = 0.0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 60000;  

extern const char* ota_psk;

void ota_init(void){
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)ota_psk);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void ota_loop(void){
  ArduinoOTA.handle();
}

void setup(){
  SERIAL_INIT
  PRINTF("My version number is: %d.%d.%d\n",FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH)
  dht.begin();

  pinMode(LED_PIN, OUTPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  PRINTLN("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    PRINTLN(".");
  }

  // Print ESP8266 Local IP Address
  PRINT("http://")
  PRINTLN(WiFi.localIP());

  webserver_init();
  mqtt_init();
  ota_init();
}
 
void loop(){  
  static uint8_t firstTime = 1;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval || firstTime) {
    firstTime = 0;
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      PRINTLN("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      PRINTLN("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
    }

    mqtt_send();
  }
  delay(250);
  mqtt_loop();
  ota_loop();
  LED_TOGGLE
}