#include <ArduinoOTA.h>
#include "common_definitions.h"
#include <Hash.h>

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
#include "app.h"

extern const char* ssid;
extern const char* password;
extern const char* ota_psk;
extern app_t app;
volatile app_timer_t timers;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 60000;  

void ota_init(void);
void ota_loop(void);
void timer_init(void);

void IRAM_ATTR onTimer() {
  if(timers.LEDBlinkTimer) timers.LEDBlinkTimer--;
  if(timers.updateDataTransmitMqttTimer) timers.updateDataTransmitMqttTimer--;
  if(timers.mqttCallLoopTimer) timers.mqttCallLoopTimer--;
  if(timers.WiFiReconnectionTimer) timers.WiFiReconnectionTimer--;
}

void setup(){
  SERIAL_INIT
  PRINTF("My version number is: %d.%d.%d\n",FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH)

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

  app_init();

  webserver_init();
  mqtt_init();
  ota_init();
  timer_init();
}
 
void loop(){     
  if(!timers.LEDBlinkTimer){
    timers.LEDBlinkTimer = TIME_LED_TOGGLE_PERIOD_100MS;
    LED_TOGGLE
  }
  if(!timers.updateDataTransmitMqttTimer){
    timers.updateDataTransmitMqttTimer = TIMER_UPDATE_DATA_MQTT_SEND_PERIOD_100MS;
    app_get_sensor_data();
    mqtt_send();
  }
  if(!timers.mqttCallLoopTimer){
    timers.mqttCallLoopTimer =  TIMER_MQTT_LOOP_CALL_PERIOD_100MS;
    mqtt_loop();
  }
  if(!timers.WiFiReconnectionTimer){
    // timers.WiFiReconnectionTimer = TIMER_WIFI_CHECK_CONNECTION_100MS;
    // CALL WIFI CHECK
  }
  // if(!timers.callOTAloopTimer){
  //   timers.callOTAloopTimer = TIME_OTA_LOOP_CALL_PERIOD_100MS;
  //   ota_loop();
  // }
  delay(10);
  ota_loop();
}

void ota_init(void){
 // Port defaults to 3232
  // ArduinoOTA.setPort(1032);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("esp32-1");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      PRINTLN("Start updating " + type)
  });
  ArduinoOTA.onEnd([]() {
      PRINTLN("\nEnd")
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef SERIAL_DEBUG
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
    LED_TOGGLE
  });
  ArduinoOTA.onError([](ota_error_t error) {
#ifdef SERIAL_DEBUG
    Serial.printf("Error[%u]: ", error);
#endif
    if (error == OTA_AUTH_ERROR) PRINTLN("Auth Failed")
    else if (error == OTA_BEGIN_ERROR) PRINTLN("Begin Failed")
    else if (error == OTA_CONNECT_ERROR) PRINTLN("Connect Failed")
    else if (error == OTA_RECEIVE_ERROR) PRINTLN("Receive Failed")
    else if (error == OTA_END_ERROR) PRINTLN("End Failed")
  });
  ArduinoOTA.begin();

  PRINTLN("Ready")
}

void ota_loop(void){
  ArduinoOTA.handle();
}

void timer_init(void){
  //Timer interrupt definition
  noInterrupts();
  timer1_isr_init();
  timer1_attachInterrupt(onTimer);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(500000);
  interrupts();

  timers.LEDBlinkTimer = TIME_LED_TOGGLE_PERIOD_100MS;
  timers.updateDataTransmitMqttTimer = 0; //TIMER_UPDATE_DATA_MQTT_SEND_PERIOD_100MS
  timers.mqttCallLoopTimer = 0; // TIMER_MQTT_LOOP_CALL_PERIOD_100MS
  timers.WiFiReconnectionTimer = 0; // TIMER_WIFI_CHECK_CONNECTION_100MS
  timers.callOTAloopTimer = TIME_OTA_LOOP_CALL_PERIOD_100MS;
}