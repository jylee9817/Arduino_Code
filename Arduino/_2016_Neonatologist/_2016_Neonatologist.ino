#include <Servo.h>
#include <SPI.h>
#include <I2C.h>

#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    maxSamples          0x0a          // Smoothing algorithm samples
#define    offset              5          // sensor offset

#define    SS_PIN   10

int readings[maxSamples];
int readIndex = 0;
int total = 0;
volatile uint16_t average = 0;

Servo lidarServo;
int position = 90; //starting position of servo(or the servo facing the middle)

int i = 0; //step for the sweep
bool sendLidarValue = 0;
volatile byte command; //varibale for the switch function - default: distance reader
volatile double joystick = 0; //joystick input from roborio

byte upperByte;
byte lowerByte;

void setup() 
{
  Serial.begin(9600);
  lidarServo.attach(9);
  lidarServo.write(90);

  //have to send on master in, "slave out"
  pinMode(MISO, OUTPUT);

  //turn on SPI in slave mode
  SPCR |= _BV(SPE);

  //now turn on interrupts
  SPCR |= _BV(SPIE);

  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails

  command = 'r';

  for(int thisReading = 0; thisReading < maxSamples; thisReading++)
  {
    readings[thisReading] = 0;
  }
}

//SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR; 

  lowerByte = lowByte(average); //average & 0xFF;
  upperByte = highByte(average); //(average & 65280)>>8;

  Serial.print("  ");
  Serial.println(c);
  
  if(c < 180 || c > 0)
  {
    joystick = c;
    delay(100);
  }
  
  switch(c)
  {
    case 190:
      if(sendLidarValue == true)
      {
        SPDR = lowerByte;
      }
      break;
    case 191:
      if(sendLidarValue == true)
      {
        SPDR = upperByte;
      }
      break;
    case 200:
      command = 'x';
      break;
    case 202:
      command = 's';
      break;
    case 201:
      command = 'u';
      break;
    case 203:
      command = 'd';
      break;
  }
  delay(100);
}

void userControl()
{
  //Serial.println(joystick);
  lidarServo.write(joystick);
  delay(100);
}
/*
void sweepRoutine()
{
 lidarServo.write(0);
 for(position = 0; position < 180; position += 1)
 {
  lidarServo.write(position);  // Move to next position
  delay(10);               // Short pause to allow it to move
  //Serial.println("servo working: ");
  //Serial.println(position);
 }
 
 for(position = 0; position >= 0; position -= 1)
 {
  lidarServo.write(position);  // Move to next position
  delay(10);               // Short pause to allow it to move
  //Serial.println("servo working: ");
  //Serial.println(position);
 }
}
*/
void sweepCheck()
{
   while(lidarServo.read() != 180)
   {   
      lidarServo.write(180);
   }
    sendLidarValue = 1;
    delay(1);
    sendLidarValue = 0;
    delay(500);
    while(lidarServo.read() != 0)
    {
      lidarServo.write(0);
    }
    sendLidarValue = 1;
    delay(1);
    sendLidarValue = 0;
    delay(500);
}
void loop() 
{
  /*if(Serial.available() > 0) //allows user input - remove once command established on roborio
  {
    command = Serial.read();
  }*/

    // LIDAR LITE ROUTINE
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

  //END OF LIDAR LITE ROUTINE

  Serial.print('x');
  Serial.println(lidarServo.read());

  //SERVO ROUTINE
  switch(command)
  {
    case 'x': //stops the servo
      sendLidarValue = false;
      lidarServo.write(90); //stops roborio
      //Serial.println("servo deactivated");
      break;

    case 'd':
      sendLidarValue = true;
      break;
      
    case 's': //scans the area infront of the robot
      sendLidarValue = false;
      sweepCheck;
      command = 'd';
      break;

    case 'u': //user control - servo moves from drive input
      userControl();
      break;
  }
  
  /*
  //if SPI is not active, clear the current command ---> stops case after one occassion
  if(digitalRead (SS_PIN) == HIGH)
  {
    command = 0;
  }
  */
}
