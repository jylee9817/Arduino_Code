#include <I2C.h>
#include <Wire.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    maxSamples          0x0a          // Smoothing algorithm samples
#define    offset              5          // sensor offset

#define    SLAVE_ADDRESS       0x78          //I2C address of the redboard

int readings[maxSamples];
int readIndex = 0;
int total = 0;
int volatile average = 0;

byte lowerByte = 0;
byte upperByte = 0;

void setup()
{
  Serial.begin(9600); //Opens serial connection at 9600bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  //Serial.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails

  pinMode(13, OUTPUT);

  //initialize i2c as slave
  //Not sure if this is necessary -> Wire.begin(SLAVE_ADDRESS);
  Wire.begin(1);

  //define callbacks for i2c communaication - not sure if this method is universal for all slave or only for rasberry pi
  Wire.onRequest(requestEvent);
  
  
  for(int thisReading = 0; thisReading < maxSamples; thisReading++)
  {
    readings[thisReading] = 0;
  }
}

void loop()
{
  // Write 0x04 to register 0x00
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0) // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
  { 
    nackack = I2c.write(LIDARLite_ADDRESS, RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    delay(1); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function
  
  // Read 2byte distance from register 0x8f
  nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0)// While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
  { 
    nackack = I2c.read(LIDARLite_ADDRESS, RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(1); // Wait 1 ms to prevent overpolling
  }
  // Serial.println(distanceArray[0]);
  // Serial.println(distanceArray[1]);
  total = total - readings[readIndex];
  readings[readIndex] = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int

  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if(readIndex >= maxSamples)
  {
    readIndex = 0;
  }

   average = (total / maxSamples)-offset;
   //Serial.write(average); Does this code needed for RoboRIO transfer?
   Serial.println(average);
   delay(1);
    
  // Print Distance
  //Serial.println(distance); //unit that it prints out is roughly in centimeters(cm)
}

void requestEvent()
{
  lowerByte = lowByte(average);
  upperByte = highByte(average);

  //Wire.beginTransmission(0); Not sure if the transmission belongs to master or slave file
  Wire.write(lowerByte);
  Wire.write(upperByte);
  //byte err = Wire.endTransmission(); //non-zero error
  
  //Wire.write(average);
  /* 
  while(1 < Wire.avaliable()) //loop through all but the last
  {
    char c = aveage;
    Serial.print(c);      // print the character
  }
  int x = average;    //receive byte as an interger
  Serial.println(x);      // print the interger
  */
}

