#include <FastLED.h>

/* ------ CONFIGURATION SETTINGS ------ */
// LED Strip
#define LED_PIN     6
#define NUM_LEDS    144
#define LED_TYPE    WS2812
#define BRIGHTNESS  100 // 0-255 value indicating scaled brightness

// colors()
#define DELAY_TIME  20
#define INCREMENT   5 // must be a divisor of 255

/* ------- GLOBALS ------ */
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, World!");

  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);

  // brightness setup
  LEDS.setBrightness(BRIGHTNESS);

  // pin setup
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  setAll(0,0,0);
  delay(1000);
  setAll(0,0,255);
  delay(1000);
}

void setAll(int r, int g, int b){
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
}







