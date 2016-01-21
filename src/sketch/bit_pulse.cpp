#include "math.h"
#include "Arduino.h"
#include "bit_pulse.h"

#define SECOND 1000000.0 // # museconds in a second
#define SAMPLE 22.7      // # museconds in a sample @ 44.1kH

BitPulse::BitPulse (uint16_t pin)
  : mNextWrite(0)
  , mTick(0)
  , mPin(pin)
  , mVal(0)
  , mWriteVal(0)
{}

void
BitPulse::play (uint32_t now, uint16_t msgLen, const uint32_t * msg)
{
  if (now >= mNextWrite)
  {
    mTick++;

    mVal = mTick * ((mTick>>msg[0]|mTick>>msg[1])&63&mTick>>msg[2]);
    analogWrite(mPin, mVal);
    mNextWrite = now + SAMPLE;
  }
}


