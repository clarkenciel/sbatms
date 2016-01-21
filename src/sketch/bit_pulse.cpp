#include "math.h"
#include "Arduino.h"
#include "bit_pulse.h"

#define SECOND 1000000.0 // # museconds in a second
#define SAMPLE 22.7      // # museconds in a sample @ 44.1kH

BitPulse::BitPulse (uint16_t pin, uint16_t mLen, const uint32_t * msg)
  : mNumWords(mLen)
  , mWords(msg)
  , mNextWrite(0)
  , mTick(0)
  , mPin(pin)
  , mVal(0)
  , mWriteVal(0)
{}

void
BitPulse::play (uint32_t now)
{
  if (now >= mNextWrite)
  {
    mVal = mTick * ((mTick>>mWords[0]|mTick>>mWords[1])&2047&mTick>>mWords[2]);
    analogWrite(mPin, mVal);
    mNextWrite = now + SAMPLE;
    mTick+=mWords[3];
  }
}

void
BitPulse::play2 ()
{
  mTick++;

  //mVal = mTick * ((mTick>>msg[0]|mTick>>msg[1])&63&mTick>>msg[2]);
  //mVal = mTick*9&mTick>>4|mTick*5&mTick>>7|mTick*3&mTick;
  //mVal = mTick*3&(mTick>>5)|mTick*3&(mTick*2>>5);
  analogWrite(mPin, mVal);
}
