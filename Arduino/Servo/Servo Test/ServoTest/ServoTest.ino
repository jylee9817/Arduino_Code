#include <Servo.h>
#include <TimerOne.h>
#include <TimerThree.h>

Servo myservo;
volatile int servoPosition = 90;
volatile byte motion = 1;

enum Mode
{
  Stop,
  Distance,
  Scan,
  ReadScan,
  ManualControl
};

Mode CurrentMode = Stop;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9);

  Serial.begin(9600);

  //Timer1.attachInterrupt(Timer_ISR, 250000);

  myservo.write(70);
  delay(150);
  myservo.write(110);
  delay(150);
  myservo.write(90);
  delay(150);

  //Timer1.initialize(100000);
  //Timer1.attachInterrupt(Timer_ISR, 250000);
  
}

void Timer_ISR()
{
  //digitalWrite(13, digitalRead(13) ^ 1);
  
  if (servoPosition > 170) motion = -2;
  if (servoPosition < 10) motion = 2;

  servoPosition =+ motion;
  
  myservo.write(servoPosition);
}

void loop() 
{
  if (Serial.available() > 0)
  { 
    byte command = Serial.read();
    
    switch (command)
    {
     case 'x':
       CurrentMode = Stop;
       Serial.println("Stopped");
       Timer1.detachInterrupt();
       break;
     case 'd':
       CurrentMode = Distance;
       Serial.println(random(10,250));
       break;
     case 's':
       CurrentMode = Scan;
       Serial.println("Scan Mode started");
       Timer3.attachInterrupt(Timer_ISR, 200000);
       break;
     case 'r':
       CurrentMode = ReadScan;
       break;
     case 'm':
       CurrentMode = ManualControl;
       break;
    }
  }
}

