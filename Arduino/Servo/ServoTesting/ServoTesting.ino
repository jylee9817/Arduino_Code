#include <Servo.h> // Note that this library disables PWM on pins 9 and 10

Servo servo1;  // servo control object

void setup()
{
  Serial.begin(9600);
  servo1.attach(9); //Begin sending control signals to the servo.   
  Serial.println("setup done");
}


void loop()
{
  int position;
  bool scan;

  // Servos can turn between 0 and 180 degrees.

for(position = 0; position < 180; position += 10)
    {
      servo1.write(position);  // Move to next position
      delay(1000);               // Short pause to allow it to move
      Serial.println("servo working: ");
      Serial.println(position);
    }
    
/*    
    // Tell servo to go to 0 degrees, stepping by one degree
  
    for(position = 180; position >= 0; position -= 10)
    {                                
      servo1.write(position);  // Move to next position
      delay(1000);               // Short pause to allow it to move
      Serial.println("servo working: ");
      Serial.println(position);
    }
*/
//    servo1.write(random(0,180));//testing
//    delay(500);
//    Serial.println("Zero");
//    servo1.write(0);//testing
//    delay(5000);

//    Serial.println("180");
//   servo1.write(180);//testing
//    delay(5000);
}

