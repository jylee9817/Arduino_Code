// what to do with incoming data
byte command = 0;
int average = 673;
byte lowerByte;
byte upperByte;

void setup ()
{
  Serial.begin(9600);
  
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;

   lowerByte = lowByte(average); //average & 0xFF;
   upperByte = highByte(average); //(average & 65280)>>8;
  
  if(c == 190)
  {
    Serial.println(c);
    SPDR = lowerByte;
  }
  
  if(c == 191)
  {
    Serial.println(c);
    SPDR = upperByte;
  }

}  // end of interrupt service routine (ISR) SPI_STC_vect

void loop ()
{
  
}  // end of loop
