# Arduino
Arduino projects created for hackathons, classes, or for fun

### Ampduino
Guitar effects box performing real-time digital signal processing using two Arduino Unos. Full code, schematics, documentation, and demonstration video available at https://github.com/jack-shirley/Ampduino

### Audio React LED cube (led_audio_react.ino)
This was a program that I wrote to accompany an LED cube my team built for the TAMUMake hackathon (documentation may be lacking since the program was written overnight). We ran 144 addressable RGB LEDs across four sides of a box and connected them to two small microphones and an Arduino Nano. The data line of the LEDs was connected to digital pin 2, and the two mics were connected to digital pin 4 and analog pin 0 to receive both digital and analog audio input. These settings as well as LED/mic calibration settings can be easily modified at the top of the program. The program contains a few different LED animations and a program to display an audio reaction to mic input amplitude (intended for music). The software can be easily scaled to LED cubes of different sizes by modifying NUM_LEDS, NUM_ROWS, and NUM_COLUMNS.

### Room Programmable Lights (room_lights_v1.ino)
Current working project, using addressable RGB LEDs and creating a controller interface to create ambient and party lighting for a living room.

### Longboard (longboard.ino)
Animations for lights affixed to the underside of my longboard, powered by batteries and controlled by a power switch and a toggle switch to change between two different animations.
