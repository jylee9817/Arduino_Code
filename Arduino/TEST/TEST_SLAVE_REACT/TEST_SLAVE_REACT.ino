#include <SPI.h>

#define    SCK_PIN  13
#define    MISO_PIN 12
#define    MOSI_PIN 11
#define    SS_PIN   10

int total = 0;
volatile byte average = 0;

byte command = 0;
//char buf [100];
//volatile byte pos;
//volatile boolean process_it;

void setup()
{
  Serial.begin(9600); //Opens serial connection at 9600bps.  

  //have to send on master in, "slave out"
  pinMode(MISO, OUTPUT);

  //turn on SPI in slave mode
  SPCR |= _BV(SPE);

  //now turn on interrupts
  SPCR |= _BV(SPIE);
}

ISR (SPI_STC_vect)
{
  byte c = SPDR;

  Serial.println('x');
  
  if(c > 0 || c < 180)
  {
    Serial.println(c);
    SPDR = 0;
  }
}

void loop()
{  

   //if SPI is not active, clear the current command
   if(digitalRead (SS_PIN) == HIGH)
   {
     command = 0;
   }
   
   //CANNOT USE SPI LIBRARY AS SLAVE
   
   delay(10); //data tranfer interval
    
  // Print Distance
  //Serial.println(distance); //unit that it prints out is roughly in centimeters(cm)
}
