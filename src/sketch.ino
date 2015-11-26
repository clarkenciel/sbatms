#include "voice.h"
#include <avr/interrupt.h>
#include <avt/pgmspace.h>

void 
setup()
{
}

void 
loop()
{
}


// GNU AVR lib documentation recommends use of ISR
ISR(TIMER1_OVF_vect)
{
  // stuff to update the table
}
