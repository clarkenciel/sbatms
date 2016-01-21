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
    BitPulse (uint16_t pin);
    ~BitPulse () {};

    void play (uint32_t now, uint16_t msgLen, const uint32_t * msg);

  private:
    uint32_t mNextWrite, mTick, mVal, mWriteVal;
    uint16_t mPin;
};

