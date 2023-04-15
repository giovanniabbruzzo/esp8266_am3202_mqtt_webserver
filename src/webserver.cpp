#include "webserver.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern volatile float t;
extern volatile float h;

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
        request->send_P(200, "text/plain", String(t).c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", String(h).c_str());
    });

    // Start server
    server.begin();
}
