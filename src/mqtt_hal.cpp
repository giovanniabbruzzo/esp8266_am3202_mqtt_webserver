#include "mqtt_hal.h"
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
char mqtt_payload[256];

extern app_t app;

// MQTT broker details
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;
extern const char* mqtt_password;
extern const char* mqtt_topic;
extern const char* mqtt_sub_topic;

int mqtt_init(void){
  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    PRINTLN("Connecting to MQTT broker...");
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password )) {
        PRINTLN("Connected to MQTT broker");
        // Subscribe
        client.subscribe(mqtt_sub_topic);
    } else {
      PRINT("Failed with state ");
      PRINT(client.state());
      return -1;
    }
  }
  return 0;
}


void mqtt_send(void){
    // Publish temperature and humidity to MQTT broker
    if (client.connected()) {
      snprintf(mqtt_payload, sizeof(mqtt_payload), 
      "{\"TP\": %.2f, \"HM\": %.2f, \"IP\":\"%s\", \"MA\":\"%s\", \"FV\":\"%s\"}", 
      app.sensors.temp, 
      app.sensors.hum,
      app.ipAddr.c_str(),
      app.macAddr.c_str(),
      app.fwVersion.c_str());
      client.publish(mqtt_topic, mqtt_payload);
      PRINT("Data sent to MQTT broker topic> ");
      PRINTLN(mqtt_topic);
      PRINT("Payload> ");
      PRINTLN(mqtt_payload);
    }else{
        mqtt_stop();
        mqtt_reconnect();
    }
}



void mqtt_callback(char* topic, byte* message, unsigned int length) {
    PRINT("Message arrived on topic: ");
    PRINT(topic);

    if (String(topic) != String(mqtt_sub_topic)){
        PRINTLN("Topic not allowed, ignoring the message")
        return;
    }

    // PRINT("Message received: ")    
    // PRINT(message)
    // Need to implement the commands yet
}


int mqtt_reconnect(void){
    // Loop until we're reconnected
    if(!client.connected()) {
        PRINT("Attempting MQTT connection...")
        return mqtt_init();
    }
    return 0;
}


void mqtt_loop(void){
    client.loop();    
}

void mqtt_stop(void){
    PRINTLN("Terminating connection with MQTT broker")
    client.disconnect();   
}