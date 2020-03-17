#include <Adafruit_NeoPixel.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "pass"
#endif

char ssid[] = STASSID;
char password[] = STAPSK;

WebServer server(80);

#define LED_PIN 0

#define NUM_PIXELS 16

// T5 == 12
#define TOUCH_PIN T5

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp32!");
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

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  //Light had one stray pixel on start up without this
  delay(100);
  
  pixels.clear();
  pixels.show();

  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //WiFi.setSleep(false);// this code solves my problem
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

  if (MDNS.begin("wfhesp")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/red", []() {
    setRingColour(pixels.Color(50, 0, 0));
    server.send(200, "text/plain", "Red Light");
  });

  MDNS.addService("http", "tcp", 80);

  server.on("/green", []() {
    setRingColour(pixels.Color(0, 50, 0));
    server.send(200, "text/plain", "Green Light");
  });

  server.on("/off", []() {
    pixels.clear();
    pixels.show();
    server.send(200, "text/plain", "Light OFF");
  });

  server.on("/touch", []() {
    String touchMessage;
    touchMessage.reserve(20);
    touchMessage = "Touch: ";
    touchMessage += touchRead(TOUCH_PIN);
    server.send(200, "text/plain", touchMessage);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}
