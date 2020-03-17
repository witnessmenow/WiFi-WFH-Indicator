#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "pass"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

#define LED_PIN 0

#define BUTTON_PIN 12

#define NUM_PIXELS 8

const char *webpage = 
#include "webPage.h"
;

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setRingColour(uint32_t colour, boolean onlyEven = false){
  pixels.clear();
  for (int i = 0; i < NUM_PIXELS; i++) { // For each pixel...

    if (!onlyEven || i % 2 == 0)
    {
      pixels.setPixelColor(i, colour);
    }

  }
  pixels.show();
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  //Light had one stray pixel on start up without this
  delay(100);
  
  pixels.clear();
  pixels.show();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("wfh")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/red", []() {
    setRingColour(pixels.Color(50, 0, 0));
    server.send(200, "text/plain", "Red Light");
  });

  server.on("/green", []() {
    setRingColour(pixels.Color(0, 50, 0));
    server.send(200, "text/plain", "Green Light");
  });

  server.on("/button", []() {
    String buttonMessage;
    buttonMessage.reserve(20);
    buttonMessage = "Button: ";
    buttonMessage += LOW == digitalRead(BUTTON_PIN);
    server.send(200, "text/plain", buttonMessage);
  });

  server.on("/off", []() {
    pixels.clear();
    pixels.show();
    server.send(200, "text/plain", "Light OFF");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
  MDNS.update();
}
