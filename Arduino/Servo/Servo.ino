#include <Servo.h>
#include <SPI.h>

#define    SS_PIN   10

Servo lidarServo;
volatile byte command = 'r'; //varibale for the switch function
int i = 0; //step
int position = 90; //starting position of servo(or the servo facing the middle)
volatile double joystick = 0; //joystick input from roborio

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
}

//SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR; 
  if(c < 180 || c > 0)
  {
    joystick = c;
    delay(100);
  }
}

void userControl()
{
  Serial.println(joystick);
  lidarServo.write(joystick);
  delay(100);
}

void sweepRoutine()
{
 lidarServo.write(0);
 for(position = 0; position < 180; position += 1)
 {
  lidarServo.write(position);  // Move to next position
  delay(10);               // Short pause to allow it to move
  Serial.println("servo working: ");
  Serial.println(position);
 }
 
 for(position = 0; position >= 0; position -= 1)
 {
  lidarServo.write(position);  // Move to next position
  delay(10);               // Short pause to allow it to move
  Serial.println("servo working: ");
  Serial.println(position);
 }
}

void loop()
{
  if(Serial.available() > 0)
  {
    command = Serial.read();
  }

  switch(command)
  {
    case 'x': //stops the servo
      lidarServo.write(90); //stops roborio
      Serial.println("servo deactivated");
      break;
      
    case 'd': //returns distance in front of the lidar 
      Serial.println(random(0,100));//should return currnet distance value
      delay(100); //delay that makes sure that the data is sent to roborio
      break;
      
    case 's': //scans the area infront of the robot
      if(i < 2)//sweeps the lidar back and forth twice for better accuracy
      {
       sweepRoutine();
       i++; 
      }
      else
      {
        command = 'r';  //the data is transfered automatically after sweep
      }
      break;
      
    case 'r': //returns array of distance data to roborio
      //SPI LIDARIONO
      i = 0; //sets step 0 for sweep
      Serial.println("filler - code running");
      delay(1000); // delay to make sure that data is successfully transferred
      lidarServo.write(90); //lidar faces middle again
      command = 'd'; //goes back to regular distance measure, if no input
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

