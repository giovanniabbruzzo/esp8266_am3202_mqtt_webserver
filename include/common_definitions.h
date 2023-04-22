#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 2
#define FW_VERSION_PATCH 3

#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
#define SERIAL_INIT Serial.begin(115200);
#define PRINT_DEBUG_INFO(str)    \
    Serial.print(millis());     \
    Serial.print(": ");    \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print(' ');      \
    Serial.print(__FILE__);     \
    Serial.print(':');      \
    Serial.print(__LINE__);     \
    Serial.print(' ');      \
    Serial.print("\"");      \
    Serial.print(str);    \
    Serial.println("\"");
#define PRINTLN(x) Serial.println(x);
#define PRINT(x) Serial.print(x);
#define PRINTF(...) Serial.printf(__VA_ARGS__);
#else
#define SERIAL_INIT 
#define PRINT_DEBUG_INFO(str) 
#define PRINTLN(x)
#define PRINT(x)
#define PRINTF(...)
#endif

#define LED_PIN LED_BUILTIN
#define LED_ON digitalWrite(LED_PIN, HIGH);
#define LED_OFF digitalWrite(LED_PIN, LOW);
#define LED_TOGGLE digitalWrite(LED_PIN, !digitalRead(LED_PIN));

// Timers
#define TIME_LED_TOGGLE_PERIOD_100MS 10
#define TIMER_UPDATE_DATA_MQTT_SEND_PERIOD_100MS 10*60
#define TIMER_MQTT_LOOP_CALL_PERIOD_100MS 3
#define TIMER_WIFI_CHECK_CONNECTION_100MS 10*60*5
#define TIME_OTA_LOOP_CALL_PERIOD_100MS 1
#define TIME_GET_SENSOR_DATA_PERIOD_100MS

#define DHTPIN D2     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

// typedef union {
//     uint8_t val;

//     struct {
//         uint8_t ledProcess : 1;
//         uint8_t otaProcess: 1;
//         uint8_t online: 1;
//         uint8_t mqttSend: 1;
//         uint8_t mqttReceived :1;
//         uint8_t wifiCheckConnection : 3;
//     };
// } app_ctx_flags_t;

struct app_timer_t{
    uint32_t LEDBlinkTimer;
    uint32_t updateDataTransmitMqttTimer;
    uint32_t mqttCallLoopTimer;
    uint32_t WiFiReconnectionTimer;
    uint32_t callOTAloopTimer;
    uint32_t getSensorDataTimer;
};

struct sensor_data_t{
    float temp;
    float hum;
};

struct app_t{
    String macAddr;
    String ipAddr;
    String fwVersion;

    // volatile app_ctx_flags_t flags;
    
    volatile sensor_data_t sensors;
};

extern app_t app;
extern volatile app_timer_t timers;