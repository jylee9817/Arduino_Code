#include <Wire.h>

#define SLAVE_ADDRESS 0x78  //I2C address of the redboard
#define MASTER_ADDRESS 0x00 //I2C address of the arduino uno

int distance = 0;
byte upperByte = 0;
byte lowerByte = 0;

void setup() 
{
  Wire.begin(MASTER_ADDRESS); //joins i2c bus (address optional for master)
  Serial.begin(9600); //start serial for output
}

void loop() 
{
  Wire.requestFrom(1, 8); //request 8 bytes from slave device #1
  
  //These are commented out to test out other method 
  //Uncommnet these after other code fails
  
  //distance = Wire.read();
  //distance <<= 8;
  //result |= Wire.read();
    
  while(Wire.available() > 0) //slave may send less than requested 
  { 
    //Wire.read();
    
    upperByte = Wire.read(); //receive a byte as character
    delay(10);
    lowerByte = Wire.read();

    distance = upperByte << 8 | lowerByte;

    Serial.print(distance); // print distance
  }

  delay(500);
}
