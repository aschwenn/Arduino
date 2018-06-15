#include <FastLED.h>

/* ------ CONFIGURATION SETTINGS ------ */
// LED Strip
#define LED_PIN     6
#define NUM_LEDS    72
#define LED_TYPE    WS2812
#define BRIGHTNESS  100 // 0-255 value indicating scaled brightness

// colors()
#define DELAY_TIME  20
#define INCREMENT   5 // must be a divisor of 255

// toggle
#define TOGGLE_PIN 2

/* ------- GLOBALS ------ */
CRGB leds[NUM_LEDS];

// colorPan()
int rval = 255;
int gval = 0;
int bval = 0;
int colorState = 0;
  
// snake()
int colors[NUM_LEDS][4];

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, World!");

  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);

  // brightness setup
  LEDS.setBrightness(BRIGHTNESS);

  // pin setup
  pinMode(LED_PIN, OUTPUT);
  pinMode(TOGGLE_PIN, INPUT);

  // initialize colors
  setAll(255,255,255);
  delay(500);
  setAll(0,0,0);
  delay(500);

  initializeColors();
}

void loop() {
  if (digitalRead(TOGGLE_PIN) == LOW){
    solidAllPan();
  }
  else snake();
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

void solidAllPan(){
  colorPan();
  setAll(rval, bval, gval);
  delay(DELAY_TIME);
}

/* SNAKE FUNCTION */

/*
 * Apparently, structs in Arduino are basically impossible
 * as there's no clear way to include a .h file...
 * 
 * My workaround is:
 * int[4] color
 * color[0]: red
 * color[1]: green
 * color[2]: blue
 * color[3]: state
 */

void colorPan2(int color[4]){
  if (color[0] == 0 && color[3] == 0) color[3] = 1;
  else if (color[1] == 0 && color[3] == 1) color[3] = 2;
  else if (color[2] == 0 && color[3] == 2) color[3] = 0;

  if (color[3] == 0){
    color[1] += INCREMENT;
    color[0] -= INCREMENT;
  }
  else if (color[3] == 1){
    color[1] -= INCREMENT; 
    color[2] += INCREMENT;
  }
  else if (color[3] == 2){
    color[2] -= INCREMENT;
    color[0] += INCREMENT;
  }
}

void initializeColors(){
  // set first LED color
  colors[0][0] = 255;
  colors[0][1] = 0;
  colors[0][2] = 0;
  colors[0][3] = 0;
  
  // iterate through our color array
  for (int i = 1; i < NUM_LEDS; i++){
    colors[i][0] = colors[i-1][0];
    colors[i][1] = colors[i-1][1];
    colors[i][2] = colors[i-1][2];
    colors[i][3] = colors[i-1][3];
    colorPan2(colors[i]);
  }
}

void snake(){
  // set all of the colors
  for (int i = 0; i < NUM_LEDS; i++){
    // iterate through all of the LEDs
    leds[i] = CRGB(colors[i][0], colors[i][1], colors[i][2]);
  }
  FastLED.show();

  // update all of the colors
  for (int i = 0; i < NUM_LEDS; i++){
    colorPan2(colors[i]);
  }

  delay(5);
}



