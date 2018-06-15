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

// colorPan()
int rval = 255;
int gval = 0;
int bval = 0;
int colorState = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, World!");

  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);

  // brightness setup
  LEDS.setBrightness(BRIGHTNESS);

  // pin setup
  pinMode(LED_PIN, OUTPUT);

  // initialize colors
  setAll(255,255,255);
  delay(500);
  setAll(0,0,0);
  delay(500);
}

void loop() {
  colorPan();
  setAll(rval, bval, gval);
  delay(DELAY_TIME);
}

void setAll(int r, int g, int b){
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
}

void colorPan(){
  // direction state
  if (rval == 0 && colorState == 0){
    // rising green
    colorState = 1; // update state to rising red
  }
  else if (gval == 0 && colorState == 1){
    // rising blue
    colorState = 2; // update state to rising blue
  }
  else if (bval == 0 && colorState == 2){
    colorState = 0; // update state to rising green
  }

  // increment/decrement colors
  if (colorState == 0){
    gval += INCREMENT;
    rval -= INCREMENT;
  }
  else if (colorState == 1){
    gval -= INCREMENT; 
    bval += INCREMENT;
  }
  else if (colorState == 2){
    bval -= INCREMENT;
    rval += INCREMENT;
  }
}





