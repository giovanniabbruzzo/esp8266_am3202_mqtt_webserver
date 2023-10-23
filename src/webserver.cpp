#include "webserver.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "app.h"

extern app_t app;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

static String processor(const String& var){
  return String();
}

void webserver_init(void){
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", String(app.sensors.temp).c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", String(app.sensors.hum).c_str());
    });
    server.on("/toggle_lamp", HTTP_GET, [](AsyncWebServerRequest *request){
        LAMP_TOGGLE        
        request->send(200);
  });

    // Start server
    server.begin();
}
