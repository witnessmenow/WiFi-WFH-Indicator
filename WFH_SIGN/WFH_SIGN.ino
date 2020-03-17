#include <Adafruit_NeoPixel.h>

#define LED_PIN 2

#define NUM_PIXELS 16

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
    
}

void loop() {
  // put your main code here, to run repeatedly:
  pixels.clear();
  for(int i=0; i<NUM_PIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if(i%2 == 0)
    {
      pixels.setPixelColor(i, pixels.Color(50, 0, 0));
    }

  }
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(5000);
  pixels.clear();
  for(int i=0; i<NUM_PIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if(i%2 == 0)
    {
      pixels.setPixelColor(i, pixels.Color(0, 50, 0));
    }

  }
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(5000);
}
