#include <FastLED.h>

/* -------- CONFIGURATION SETTINGS -------- */
#define LED_PIN     5
#define WALL_1      14
#define WALL_2      0
#define WALL_3      0
#define WALL_4      0

#define SELECTION_PIN 0

/* -------- GLOBALS -------- */
CRGB leds[WALL_1 + WALL_2 + WALL_3 + WALL_4];
int LED_COUNT[4];

void setup() {
  Serial.begin(9600);

  // Assign LED wall counts
  LED_COUNT[0] = WALL_1;
  LED_COUNT[1] = WALL_2;
  LED_COUNT[2] = WALL_3;
  LED_COUNT[3] = WALL_4;
  
  // Selection must be made before turning on the controller
  // TODO: Read SELECTION_PIN value

  // Button setup
  pinMode(LED_PIN, OUTPUT);
  pinMode(SELECTION_PIN, INPUT);
  // TODO: set up audio input pins
  //setAll(255,255,255,0);
  leds[0] = CRGB(255,255,255);
  FastLED.show();
}

void loop() {
  
}

/*  Set all pins on a wall to a certain color
 *  Inputs:
 *    r, g, b (0-255 RGB values)
 *    wall (0, 1, 2, or 3)
 */
void setAll(int r, int g, int b, byte wall){
  int startRange = 0;
  for (int i = 0; i < wall; i++){
    startRange += LED_COUNT[i];
  }
  int endRange = startRange + LED_COUNT[wall];
  for (int i = startRange; i < endRange; i++){
    leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
}

/*  Reads control pot to see which one of 11 settings is currently selected
 */
int controlRead(){
  int err = 39;
  int in = 0;
  int sampleSize = 10;
  int values[] = {1023,980,858,735,621,510,396,278,156,35};

  // read selector pin
  for(int i=0; i<sampleSize; i++){
    in += analogRead(selector);
  }
  in /= sampleSize;
  //Serial.println(in);

  // determine which mode is selected
  for(int j=0; j<10; j++){
    if((in > (values[j] - err)) && (in < (values[j] + err))){
      return j;
    }
  }
  return -1;
}
