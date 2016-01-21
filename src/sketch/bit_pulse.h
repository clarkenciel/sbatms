/*
 * bit_pulse.h
 * 
 * Bit logic based synthesis per the work of Viznut:
 * http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
 *
 * Author: Danny Clarke
 */

#pragma once

#include <stdint.h>

class BitPulse
{
  public:
    BitPulse (uint16_t pin, uint16_t numWords, const uint32_t * words);
    ~BitPulse () {};

    void play (uint32_t now);
    void play2 ();

  private:
    uint32_t mNextWrite, mTick, mVal, mWriteVal;
    uint16_t mPin, mNumWords;
    const uint32_t * mWords;
};


