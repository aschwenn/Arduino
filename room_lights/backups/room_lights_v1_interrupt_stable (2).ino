#include <FastLED.h>

/* -------- CONFIGURATION SETTINGS -------- */
// LEDs
#define LED_PIN         8
#define NUM_LEDS        14
#define LED_TYPE        WS2812

// buttons
#define PRIMARY_TOGGLE    3
#define SECONDARY_TOGGLE  2
#define MAX_OPTIONS       1

// brightness
void adjustBrightness();
#define BRIGHTNESS      50  // 0-255 value indicating scaled brightness, used for constant brightness
#define BRIGHTNESS_PIN  A0  // analog input pin
#define BRIGHTNESS_POT  true  // if using a potentiometer to adjust brightness

// colorchain()
void colorchain(int chainlength);
#define DELAYTIME       20
#define INC             5

/* -------- GLOBALS -------- */
CRGB leds[NUM_LEDS];
volatile int primaryOption = 0;
volatile int secondaryOption = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello World!\n");

  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);

  // brightness setup
  if (!BRIGHTNESS_POT){ // if not using a potentiometer to control brightness, set constant value
    LEDS.setBrightness(BRIGHTNESS); // must be applied BEFORE setting LED colors
  }

  // button setup
  pinMode(PRIMARY_TOGGLE,INPUT);
  attachInterrupt(digitalPinToInterrupt(PRIMARY_TOGGLE),primaryToggle,RISING);
}

void loop() {  
  if (primaryOption == 0) setAll(255,255,255);
  else if (primaryOption == 1) colorchain(4);
}

/* MAIN DISPLAY FUNCTIONS
 *  void colorchain(int chainlength, int cycles)
 *    code reuse from hackathon project, creates snaking effect
 *    pass in integer value for 'cycles', otherwise it will run continuously
*/

void colorchain(int chainlength){
  int rval = 255;
  int bval = 0;
  bool forwards = true;
  
  for (int i = 0; i < NUM_LEDS; i++){
    for (int j = 0; j < chainlength; j++){
      // iterates through chain
      if (i+j > NUM_LEDS){
        // wrapping
        leds[(i+j)-NUM_LEDS] = CRGB(rval,0,bval);
        FastLED.show();
      }
      else{
        leds[i+j] = CRGB(rval,0,bval);
        FastLED.show();
      }
      delay(DELAYTIME);
      adjustBrightness();
    }
    if (i == 0){
      leds[NUM_LEDS - 1] = CRGB::Black;
      FastLED.show();
    }
    else{
      leds[i-1] = CRGB::Black;
      FastLED.show();
    }
  
    // direction
    if (rval == 0) forwards = false;
    else if (bval == 0) forwards = true;
    
    if (forwards){
      rval -= INC;
      bval += INC;
    }
    else{
      rval += INC;
      bval -= INC;
    }
  }
}

/* AUXILIARY FUNCTIONS 
 *  setAll(int r, int g, int b)
 *    sets all LEDs to the same color
*/

void setAll(int r, int g, int b){
  // sets all LEDs to a certain color
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
}

void adjustBrightness(){
  if (BRIGHTNESS_POT){ // if using a potentiometer to adjust brightness
    int bIn = 1023 - analogRead(BRIGHTNESS_PIN); // read in; swap value so that turning down the knob turns down the brightness
    LEDS.setBrightness(map(bIn, 0, 1023, 0, 255)); // maps from analog 0-1023 to 0-255 value usable by function
  }
}

void colorPan(){
  
}

void primaryToggle(){
  noInterrupts(); // block interrupts momentarily
  
  if(primaryOption == MAX_OPTIONS) primaryOption = 0;
  else primaryOption++;

  Serial.print("PRIMARY TOGGLE: Switched to option ");
  Serial.println(primaryOption);

  delay(500);

  interrupts(); // unblock interrupts
}

