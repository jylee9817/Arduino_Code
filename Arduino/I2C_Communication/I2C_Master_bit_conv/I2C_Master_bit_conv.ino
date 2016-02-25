
// Written by Nick Gammon
// February 2011

#include <Wire.h>

const byte MY_ADDRESS = 25;      // me
const byte SLAVE_ADDRESS = 42;   // slave #42

void setup () 
  {
  Wire.begin (MY_ADDRESS);  // initialize hardware registers etc.
  }  // end of setup

void loop() 
  {
  unsigned int value = 1234;  // ie. 0x04 0xD2
  
  Wire.beginTransmission (0);  // broadcast to all
  Wire.write (highByte (value));   
  Wire.write (lowByte (value)); 
  byte err = Wire.endTransmission  ();  // non-zero means error
      
  delay (100);  // wait 0.1 seconds
  }   // end of loop
