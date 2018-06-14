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
void colorchain();
#define CHAINLENGTH     4
#define DELAYTIME       20
#define INC             5

// colorPan()
#define COLOR_DELAY     25

/* -------- GLOBALS -------- */
CRGB leds[NUM_LEDS];
int rval = 0;
int gval = 0;
int bval = 0;

/* -------- THREADING -------- */
/* Auxiliary threads (not associated with a main display function):
 *  brightnessThread - checks potentiometer output to manipulate LED brightness
 *  colorPanThread - pans through all colors
 *  
 *  
 * A caveat on using the ArduinoThread library:
 * Functions with input arguments can't be run in Threads. Therefore, any function
 * with a parameter that needs to be passed in may have to have the parameter set as a global.
 * 
 * Default maximum number of threads is 15
 */
ThreadController control = ThreadController(); // used to switch between threads
Thread brightnessThread = Thread(); // instantiating the thread
Thread colorPanThread = Thread();

void setup() {
  Serial.begin(9600);
  Serial.println("HELLO WORLD!\n");

  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  
  if (!BRIGHTNESS_POT){ // if not using a potentiometer to control brightness, set constant value
    LEDS.setBrightness(BRIGHTNESS); // must be applied BEFORE setting LED colors
  }
  Serial.println("LEDs initialized.");

  // Thread setup
  brightnessThread.onRun(adjustBrightness); // function that the thread will call upon running
  brightnessThread.setInterval(200); // how long the thread should run for
  colorPanThread.onRun(colorPan);
  colorPanThread.setInterval(500);
  Serial.println("Auxiliary threads set up.");
  
  // PLACE CURRENT TESTING THREADS HERE
  Thread colorchainThread = Thread();
  colorchainThread.onRun(colorchain);
  colorchainThread.setInterval(1000);
  Serial.println("Other threads set up.");

  // Thread controller setup
  control.add(&brightnessThread); // for non-pointers, must be passed as a reference
  control.add(&colorPanThread);
  control.add(&colorchainThread);
  Serial.println("Thread controller set up.");
}

void loop() {
  Serial.print(millis());
  Serial.println("ms: Running controller...");
  control.run();
}

/* MAIN DISPLAY FUNCTIONS
 *  void colorchain(int cycles)
 *    code reuse from hackathon project, creates snaking effect
 *    pass in integer value for 'cycles', otherwise it will run continuously
*/

void colorchain(){
  Serial.print("colorchainThread's turn! ");

  for (int i = 0; i < NUM_LEDS; i++){
    for (int j = 0; j < CHAINLENGTH; j++){
      // iterates through chain
      if (i+j > NUM_LEDS){
        // wrapping
        leds[(i+j)-NUM_LEDS] = CRGB(rval,gval,bval);
      }
      else{
        leds[i+j] = CRGB(rval,gval,bval);
      }
      FastLED.show();
      delay(DELAYTIME);
    }
    if (i == 0){
      leds[NUM_LEDS - 1] = CRGB::Black;
    }
    else{
      leds[i-1] = CRGB::Black;
    }
    FastLED.show();
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
  Serial.print("brightnessThread's turn! ");

  if (BRIGHTNESS_POT){ // if using a potentiometer to adjust brightness
    int bIn = 1023 - analogRead(BRIGHTNESS_PIN); // read in; swap value so that turning down the knob turns down the brightness
    LEDS.setBrightness(map(bIn, 0, 1023, 0, 255)); // maps from analog 0-1023 to 0-255 value usable by function
  }
}

void colorPan(){
  Serial.print("colorPanThread's turn! ");
  
  rval = 255;
  bval = 0;
  bool forwards = true;

  while(true){
    if (forwards){
      rval -= INC;
      bval += INC;
    }
    else{
      rval += INC;
      bval -= INC;
    }
    
    // direction
    if (rval == 0) forwards = false;
    else if (bval == 0) forwards = true;
    
    delay(COLOR_DELAY);
  }
}

