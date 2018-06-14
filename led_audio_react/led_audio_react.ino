#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    144
#define LED_TYPE    WS2812
#define BRIGHTNESS  100

// sound detector
#define ANALOG_PIN      0
#define DIGITAL_PIN     4
#define BAUD_RATE       9600
#define MAX_AMPLITUDE   850     // actual max is 1023, modified to calibrate
#define BIN_SIZE        100     // number of amplitude values averaged to reduce noise
#define NOISE_LOW_BOUND 2       // amplitudes below this ignored
#define NOISE_UP_BOUND  800     // amplitudes above this ignored
#define MIN_SENSITIVITY 0.7     // sensitivity is reduced by this value at the minimum (top row)
#define AMP_CUTOFF      0.75    // the cutoff amplitude for the highest row

// colorchain()
#define DELAYTIME   60
#define INC         5

// box details
#define NUM_ROWS    6
#define NUM_COLUMNS 6


CRGB leds[NUM_LEDS];

/*
 * Usable animations:
 * colorchain(int chainlength, int cycles)
 * fadeUpward(int cycles)
 * fadeSideways(int cycles)
 * text(int cycles)
 */



void setup() {
  // LED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);

  // MIC setup
  Serial.begin(BAUD_RATE);
  pinMode(DIGITAL_PIN, INPUT);

  // hello
  setAll(0,0,0);
  delay(500);
  setAll(255,255,255);
  loading();
  delay(500);
  setAll(0,0,0);
  delay(500);
}

void loop() {
  lightsShow(2);
  soundResponse();
}

void loading(){
  int load[] = {27,26,19,13,8,9,16,22};
  for (int w = 0; w < 2; w++){
    for (int i = 0; i < 3; i++){
      setAll(255,255,255);
      for (int j = 0; j < 4; j++){
        leds[load[i] + (NUM_ROWS * NUM_COLUMNS)*j] = CRGB::Black;
      }
      FastLED.show();
      delay(50);
    }
  }
  setAll(255,255,255);
}

/* AUDIO FUNCTIONS */

void soundResponse(){
  // constant light, brightness varies based on sound amplitude
  double amp = 0; // multiplier to scale the brightness based on amplitude
  int temp = 0;
  int temp2 = 0;

  /*
   * Colors change upwards from blue to red
   * Lower rows are more responsive to sound, upper rows are less responsive
   */
   int colorStep = 255 / (NUM_ROWS - 1);
   double sensitivityStep = double(double(MIN_SENSITIVITY) / double(NUM_ROWS - 1));
   double cutoffStep = double((1 - AMP_CUTOFF) / double(NUM_ROWS - 1));
  
  while(1){
    // take a sample of values
    amp = 0;
    for (int i = 0; i < BIN_SIZE; i++){
      temp = analogRead(ANALOG_PIN);
      temp2 = digitalRead(DIGITAL_PIN);
      if (temp < NOISE_LOW_BOUND){
        // if low noise, don't record value
        i--;
        continue;
      }
      if (temp > NOISE_UP_BOUND){
        // if high noise, don't record value
        i--;
        continue;
      }
      if (temp2 == HIGH){
        // if input is high, disregard
        i--;
        continue;
      }
      amp += double(temp / double(MAX_AMPLITUDE)); // will be a value from 0 to 1
      //Serial.println(temp);
    }
    // average the values
    amp = amp / BIN_SIZE;
    //Serial.println(amp * 100); // For debugging purposes

    //setAll(0,255 * amp,0); // Use for monochromatic

    // Color fade - blue to red
    for (int j = 0; j < NUM_ROWS; j++){
      //setRow((0 + (colorStep * j)) * amp, (255 - (colorStep * j)) * amp, j);
      //setRow(((0 + (colorStep * j)) * amp) * (1 - (sensitivityStep * j)), ((255 - (colorStep * j)) * amp) * (1 - (sensitivityStep * j)), j); // sensitivity-based

      // cutoff-based solution
      if (amp > (AMP_CUTOFF - (cutoffStep * (NUM_ROWS - j)))){
        // if the frequency is greater than the row's cutoff value
        setRow((0 + (colorStep * j)) * amp, (255 - (colorStep * j)) * amp, j);
      }
      else setRow(0,0,j);
      
      //Serial.println(sensitivityStep * j); // For debugging purposes
    }
    /*
    // Shape overlays
    int heart[] = {1,10,16,13,22,20,18,26,24,35,33};
    for (int w = 0; w < 4; w++){
      for (int h = 0; h < 11; h++){
        leds[heart[h] + (NUM_ROWS * NUM_COLUMNS * w)] = CRGB::Black;
      }
    }
    */
    FastLED.show();
  }
}

void micScan(){
  // prints mic inputs
  int val;
  while(1){
    val = analogRead(ANALOG_PIN);
    Serial.println(val);
  }
}

/* COLOR PATTERNS */

void lightsShow(int cycles){
  for (int i = 0; i < cycles; i++){
    fadeUpwards(18);
    fadeSideways(18);
  }
}

void setAll(int r, int g, int b){
  // sets all LEDs to a certain color
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(r,g,b);
  }
  FastLED.show();
}

void blinkColor(int r, int g, int b){
  // blink a certain color
  while(1){
    setAll(r,g,b);
    delay(50);
    setAll(0,0,0);
    delay(50);
  }
}

void colorchain(int chainlength, int cycles){
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
    
    delay(DELAYTIME);
    // keep loop running forever by preventing the counter from incrementing
    if (infinite) w--;
  }
}

void fadeUpwards(int cycles){
  // cycles = number times a full cycle will execute
  // 0 cycles = infinite
  
  int rval = 255;
  int bval = 0;
  bool forwards = true;

  // cycle check
  bool infinite = false;
  if (cycles == 0){
    infinite = true;
    cycles = 1;
  }

  int DIMMER = 3; // amount to divide RGB values by to dim

  for (int w = 0; w < cycles; w++){
    for (int i = 0; i < NUM_ROWS; i++){
      // light all LEDs on a certain row, on each face, simultaneously
      
      setRow(rval,bval,i); // set row for row i using RGB = (rval,0,bval)
      
      // set next and previous rows to same color with half intensity
      // previous
      if (i == 0) setRow(rval/DIMMER,bval/DIMMER,NUM_ROWS-1);
      else setRow(rval/DIMMER,bval/DIMMER,i-1);
      // next
      if (i == (NUM_ROWS - 1)) setRow(rval/DIMMER,bval/DIMMER,0);
      else setRow(rval/DIMMER,bval/DIMMER,i+1);      

      // set 2 back and 2 forward to off
      // 2 back
      if (i == 1) setRow(0,0,NUM_ROWS-1);
      else if (i == 0) setRow(0,0,NUM_ROWS-2);
      else setRow(0,0,i-2);
      // 2 forward
      if (i == (NUM_ROWS - 2)) setRow(0,0,0);
      else if (i == (NUM_ROWS - 1)) setRow(0,0,1);
      else setRow(0,0,i+2);
      
      FastLED.show();

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
  
      delay(DELAYTIME);
    }

    // keep loop running forever by preventing the counter from incrementing
    if (infinite) w--;
  }
}

void fadeSideways(int cycles){
  // cycles = number times a full cycle will execute
  // 0 cycles = infinite
  
  int rval = 255;
  int bval = 0;
  bool forwards = true;

  // cycle check
  bool infinite = false;
  if (cycles == 0){
    infinite = true;
    cycles = 1;
  }

  int DIMMER = 3; // amount to divide RGB values by to dim

  for (int w = 0; w < cycles; w++){
    for (int i = 0; i < NUM_COLUMNS; i++){
      // light all LEDs on a certain row, on each face, simultaneously
      
      setColumn(rval,bval,i); // set row for row i using RGB = (rval,0,bval)
      
      // set next and previous rows to same color with half intensity
      // previous
      if (i == 0) setColumn(rval/DIMMER,bval/DIMMER,NUM_COLUMNS-1);
      else setColumn(rval/DIMMER,bval/DIMMER,i-1);
      // next
      if (i == (NUM_COLUMNS - 1)) setColumn(rval/DIMMER,bval/DIMMER,0);
      else setColumn(rval/DIMMER,bval/DIMMER,i+1);      

      // set 2 back and 2 forward to off
      // 2 back
      if (i == 1) setColumn(0,0,NUM_COLUMNS-1);
      else if (i == 0) setColumn(0,0,NUM_COLUMNS-2);
      else setColumn(0,0,i-2);
      // 2 forward
      if (i == (NUM_COLUMNS - 2)) setColumn(0,0,0);
      else if (i == (NUM_COLUMNS - 1)) setColumn(0,0,1);
      else setColumn(0,0,i+2);
      
      FastLED.show();

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
  
      delay(DELAYTIME);
    }

    // keep loop running forever by preventing the counter from incrementing
    if (infinite) w--;
  }
}

void setColumn(int rval, int bval, int column){
  // auxiliary function
  // fills all rows for specified column on all 4 faces
  int sideTotal = NUM_ROWS * NUM_COLUMNS;

  for (int j = 0; j < NUM_ROWS; j++){
    leds[column + (j * NUM_ROWS)] = CRGB(rval,0,bval);
    leds[column + sideTotal + (j * NUM_ROWS)] = CRGB(rval,0,bval);
    leds[column + (sideTotal * 2) + (j * NUM_ROWS)] = CRGB(rval,0,bval);
    leds[column + (sideTotal * 3) + (j * NUM_ROWS)] = CRGB(rval,0,bval);
  }
}

void setRow(int rval, int bval, int row){
  // auxiliary function
  // fills all columns for specified row on all 4 faces
  int rowOffset = row * NUM_COLUMNS;
  int sideTotal = NUM_ROWS * NUM_COLUMNS;
  
  for (int j = 0; j < NUM_COLUMNS; j++){
    leds[rowOffset + j] = CRGB(rval,0,bval);
    leds[rowOffset + sideTotal + j] = CRGB(rval,0,bval);
    leds[rowOffset + (sideTotal * 2) + j] = CRGB(rval,0,bval);
    leds[rowOffset + (sideTotal * 3) + j] = CRGB(rval,0,bval);
  }
}

void text(int cycles){
  // Easter egg that has the letters TAMUMAKE pan across the cube
  int rval = 255;
  int bval = 0;
  bool forwards = true;

  int TEXTDELAY = 700;
  
  // cycle check
  bool infinite = false;
  if (cycles == 0){
    infinite = true;
    cycles = 1;
  }

  int sideTotal = NUM_ROWS * NUM_COLUMNS;

  for (int w = 0; w < cycles; w++){
    for (int i = 0; i < NUM_LEDS; i++){
      showT(rval,bval,0);
      showA(rval,bval,3*sideTotal);
      showM(rval,bval,2*sideTotal);
      showU(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showA(rval,bval,0);
      showM(rval,bval,3*sideTotal);
      showU(rval,bval,2*sideTotal);
      showM(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showM(rval,bval,0);
      showU(rval,bval,3*sideTotal);
      showM(rval,bval,2*sideTotal);
      showA(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showU(rval,bval,0);
      showM(rval,bval,3*sideTotal);
      showA(rval,bval,2*sideTotal);
      showK(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showM(rval,bval,0);
      showA(rval,bval,3*sideTotal);
      showK(rval,bval,2*sideTotal);
      showE(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showA(rval,bval,0);
      showK(rval,bval,3*sideTotal);
      showE(rval,bval,2*sideTotal);
      showT(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showK(rval,bval,0);
      showE(rval,bval,3*sideTotal);
      showT(rval,bval,2*sideTotal);
      showA(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);

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

      showE(rval,bval,0);
      showT(rval,bval,3*sideTotal);
      showA(rval,bval,2*sideTotal);
      showM(rval,bval,sideTotal);
      FastLED.show();
      delay(TEXTDELAY);
    
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

void clearFace(int offset){
  for (int i = 0; i < NUM_ROWS * NUM_COLUMNS; i++){
    leds[i + offset] = CRGB(0,0,0);
  }
}

void showT(int rval, int bval, int offset){
  int ind[] = {30,31,32,33,34,35,24,25,26,27,28,29,20,21,14,15,8,9,2,3};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showA(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,4,5,10,11,16,17,19,22,26,27,32,33,14,15};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showM(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,4,5,10,11,16,17,22,23,28,29,34,35,26,27,20,21};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showU(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,4,5,10,11,16,17,22,23,28,29,34,35,8,9,2,3};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showH(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,4,5,10,11,16,17,22,23,28,29,34,35,14,15,20,21};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showC(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,2,3,4,5,8,9,10,11,32,33,34,35,26,27,28,29};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showK(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,20,21,14,15,28,29,34,35,10,11,4,5};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

void showE(int rval, int bval, int offset){
  int ind[] = {0,1,6,7,12,13,18,19,24,25,30,31,2,3,4,5,14,15,16,17,32,33,34,35,20,21,22,23};
  clearFace(offset);
  for (int i = 0; i < (sizeof(ind)/sizeof(ind[0])); i++){
    leds[ind[i] + offset] = CRGB(rval,0,bval);
  }
}

