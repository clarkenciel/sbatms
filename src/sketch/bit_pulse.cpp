#include "math.h"
#include "Arduino.h"
#include "bit_pulse2.h"
#include "pulse.h"

#define SECOND 1000000.0 // # museconds in a second
#define SAMPLE 22.7      // # museconds in a sample @ 44.1kH

BitPulse::BitPulse (uint16_t pin)
  : mNextWrite(0)
  , mTick(0)
  , mPin(pin)
  , mVal(0)
{}

void
BitPulse::play (uint32_t now)
{
  if (now >= mNextWrite)
  {
    mTick++;
    mVal = ((sin(mTick >> 1) + cos(mTick >> 2))
        + (mTick >> (mTick | 1))) * 5;
    analogWrite(mPin, mVal);
    mNextWrite = now + SAMPLE;
  }
}

