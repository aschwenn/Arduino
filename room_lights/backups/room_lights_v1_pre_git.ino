#include <FastLED.h>

/* -------- CONFIGURATION SETTINGS -------- */
// LEDs
#define LED_PIN         8
#define NUM_LEDS        14
#define LED_TYPE        WS2812
void colorPan();

// buttons
#define PRIMARY_TOGGLE    3
#define SECONDARY_TOGGLE  2
#define MAX_OPTIONS       2
#define TOGGLE_BUFFER     200 // must be this many ms between button presses to register

// brightness
void adjustBrightness();
#define BRIGHTNESS      50  // 0-255 value indicating scaled brightness, used for constant brightness
#define BRIGHTNESS_PIN  A0  // analog input pin
#define BRIGHTNESS_POT  true  // if using a potentiometer to adjust brightness

// colorchain()
void colorchain(int chainlength);
#define DELAYTIME       20
#define INC             3 // must be a divisor of 255 (factors of 255: 1,3,5,15,17,51,85,255)

// audio react
#define MIC_PIN         0

/* -------- GLOBALS -------- */
CRGB leds[NUM_LEDS];

// Interrupts
volatile int primaryOption = 0;
volatile int secondaryOption = 0;
volatile int lastPToggle = 0; // time of last primary toggle
bool interrupted = false;

// color panning
byte rval = 255;
byte gval = 0;
byte bval = 0;
byte colorState = 0; // 0 for falling red/rising green, 1 for falling green/rising blue, 2 for falling blue/rising red

void setup() {
  //Serial.begin(9600);
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

  // audio react setup
  pinMode(MIC_PIN,INPUT);
}

void loop() {
  setAll(0,0,0);
  if (primaryOption == 0) setAll(255,255,255);
  else if (primaryOption == 1) colorchain(floor(NUM_LEDS / 4));
  else if (primaryOption == 2) standardAudioReact();
}

/* MAIN DISPLAY FUNCTIONS
 *  void colorchain(int chainlength, int cycles)
 *    code reuse from hackathon project, creates snaking effect
 *    pass in integer value for 'cycles', otherwise it will run continuously
*/

void standardAudioReact(){
  Serial.println("Audio react initiated.");
  while(true){
    //Serial.println(String(analogRead(MIC_PIN)));
    if (interrupted){
      interrupted = false;
      return;
    }
  }
}

void colorchain(int chainlength){
  while(true){
    for (int i = 0; i < NUM_LEDS; i++){
      for (int j = 0; j < chainlength; j++){
        // iterates through chain
        if (interrupted){
          interrupted = false;
          return;
        }
        colorPan(); // update colors
        int r = rval;
        int g = gval; // without creating these buffer variables, the globals get corrupted somehow
        int b = bval;
        if (i+j > NUM_LEDS){
          // wrapping
          leds[(i+j)-NUM_LEDS] = CRGB(r,g,b);
        }
        else{
          leds[i+j] = CRGB(r,g,b);
        }
        FastLED.show();
        delay(DELAYTIME);
        adjustBrightness();
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
  adjustBrightness();
  FastLED.show();
}

void adjustBrightness(){
  if (BRIGHTNESS_POT){ // if using a potentiometer to adjust brightness
    int bIn = 1023 - analogRead(BRIGHTNESS_PIN); // read in; swap value so that turning down the knob turns down the brightness
    LEDS.setBrightness(map(bIn, 0, 1023, 0, 255)); // maps from analog 0-1023 to 0-255 value usable by function
  }
}

void colorPan(){
  // direction state
  if (rval == 0 && colorState == 0){
    colorState = 1;
    gval = 255;
  }
  else if (gval == 0 && colorState == 1){
    colorState = 2;
    bval = 255;
  }
  else if (bval == 0 && colorState == 2){
    colorState = 0;
    rval = 255;
  }

  // increment/decrement colors
  if (colorState == 0){
    gval = gval + INC;
    rval = rval - INC;
    bval = 0;
  }
  else if (colorState == 1){
    gval = gval - INC;
    bval = bval + INC;
    rval = 0;
  }
  else if (colorState == 2){
    bval = bval - INC;
    rval = rval + INC;
    gval = 0;
  }
}

void primaryToggle(){
  noInterrupts(); // block interrupts momentarily

  if (millis() - lastPToggle > TOGGLE_BUFFER){
    if(primaryOption == MAX_OPTIONS) primaryOption = 0;
    else primaryOption++;

    lastPToggle = millis(); // set to the current time
    interrupted = true; // in order to exit current function
  }

  Serial.print("PRIMARY TOGGLE: Switched to option ");
  Serial.print(primaryOption);
  Serial.print(" at " + String(millis()) + " ms.\n");

  interrupts(); // unblock interrupts
}

