# Arduino
Arduino projects created for hackathons or for fun

### Audio React LED cube (led_audio_react.ino)
This was a program that I wrote to accompany an LED cube my team built for the TAMUMake hackathon. We ran 144 addressable RGB LEDs across four sides of a box and connected them to two small microphones and an Arduino Nano. The data line of the LEDs was connected to digital pin 2, and the two mics were connected to digital pin 4 and analog pin 0 to receive both digital and analog audio input. These settings as well as LED/mic calibration settings can be easily modified at the top of the program. The program contains a few different LED animations and a program to display an audio reaction to mic input amplitude (intended for music). The software can be easily scaled to LED cubes of different sizes by modifying NUM_LEDS, NUM_ROWS, and NUM_COLUMNS.
