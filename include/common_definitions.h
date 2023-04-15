#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.h"

#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 1
#define FW_VERSION_PATCH 2

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

extern volatile float t;
extern volatile float h;