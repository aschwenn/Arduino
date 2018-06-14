#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>

#include <FastLED.h>

/* -------- CONFIGURATION SETTINGS -------- */
#define LED_PIN         2
#define NUM_LEDS        14
#define LED_TYPE        WS2812

// brightness
void adjustBrightness();
#define BRIGHTNESS      50  // 0-255 value indicating scaled brightness, used for constant brightness
#define BRIGHTNESS_PIN  A0  // analog input pin
#define BRIGHTNESS_POT  true  // if using a potentiometer to adjust brightness

// colorchain()
void colorchain(int chainlength, int cycles = 0);
#define DELAYTIME       20
#define INC             5

/* -------- GLOBALS -------- */
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  
  if (!BRIGHTNESS_POT){ // if not using a potentiometer to control brightness, set constant value
    LEDS.setBrightness(BRIGHTNESS); // must be applied BEFORE setting LED colors
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  colorchain(4);
}

/* MAIN DISPLAY FUNCTIONS
 *  void colorchain(int chainlength, int cycles)
 *    code reuse from hackathon project, creates snaking effect
 *    pass in integer value for 'cycles', otherwise it will run continuously
*/

void colorchain(int chainlength, int cycles = 0){
  int rval = 255;
  int bval = 0;
  bool forwards = true;
  
  // cycle check
  bool infinite = false;
  if (cycles == 0){
    infinite = true;
    cycles = 1;
  }

  for (int w = 0; w < cycles; w++){
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
    
    // keep loop running forever by preventing the counter from incrementing
    if (infinite) w--;
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

