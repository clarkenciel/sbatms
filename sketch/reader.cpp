#include "reader.h"  
#include "Arduino.h"

Reader::Reader (uint16_t pin, uint32_t msgDelta, uint32_t timeOutInterval)
  : mPin(pin)
  , mMsgDelta(msgDelta)
  , mTimeOutInterval(timeOutInterval)
  , mCurrentVal(0)
  , mLastVal(0) {
   mState = UP_LISTENING;  
}

/*
 * A change in the state of the pins can be detected while
 * the reader is one of two states, UP_LISTENING or DOWN_LISTENING.
 * If the pins change while UP_LISTENING, then we mark the start time
 * of an UP period and change state to DOWN_LISTENING.
 * If the pins while DOWN_LISTENING, then we mark the end time of an
 * UP period and, if the period was long enough, we change state to WORD_READ
 */
void Reader::read2 (uint32_t now, const uint8_t pinRegister, const uint8_t pins) {
  switch (mState) {

    // Initial state: watching for a light to turn on.
  case UP_LISTENING:
    bool shift = detectShift2(pinRegister, pins);
    if (shift) {
      mStart = now;
      mState = DOWN_LISTENING;
      mTimeOut = now + mTimeOutInterval;
    }
    break;

    // Saw a light turn on, waiting for it to turn off.
  case DOWN_LISTENING:
    bool shift = detectShift2(pinRegister, pins);

    // record duration between light turning on and turning off
    // as a word.
    // TODO: How to handle similar behavior if the light is
    //       simply moved? Perhaps invert this machine to
    //       record period with no light?
    if (shift) {
      mEnd = now;
      mWord = (mEnd - mStart) / mMsgDelta;      
      if (mWord > 0)
        mState = WORD_READ;
    }
    
    // shift back to UP_LISTENING on timeout
    else if (now > mTimeOut)
      mState = UP_LISTENING;
    break;

    // only move back to UP_LISTENING when the word is taken
  case WORD_TAKEN:
    mState = UP_LISTENING;
    break;
    
    // only other state is WORD_READ, do nothing in that state
  default:
    break;
  }
}

/*
 * Verify whether the reader has read a word
 * that has not been taken already
 */
bool Reader::hasWord () {
  switch(mState) {
  case WORD_READ:
    return true;
  default:
    return false;
  }
}

/*
 * Return the word and mark the reader as read
 */
uint32_t Reader::getWord () {
  mState = WORD_TAKEN;
  return mWord;
}

/*
 * Detects shifts on the pins in the pin register.e
 * "pinRegister" is the byte representing the current status of some
 * of the pins on an arduino (e.g. PIND or PINC).
 * "pins" is a filter to specify which pins on that register we want
 * to be checking.
 */
bool Reader::detectShift2 (const uint8_t pinRegister, const uint8_t pins) { 
  // if the pins we care about are off,
  // then mCurrentVal will be 0
  // Otherwise mCurrentVal will be some number.
  mCurrentVal = pinRegister & pins;  
  
  //   lo -> hi                  ||  hi -> lo
  if ((mCurrentVal && !mLastVal) || (!mCurrentVal && mLastVal)) {
    mLastVal = mCurrentVal;
    return true;
  }
  else {
    mLastVal = mCurrentVal;
    return false;
  }
}

/* ------------- DEPRECATED METHODS ------------- */
/*
 * Words correspond to the length of time
 * between intensity changes in the incoming signal (see detectShift).
 * This will record those words using an input time
 */
/*
void Reader::read (uint32_t now) {
   bool shift = detectShift();
   switch (mState) {
      case UP_LISTENING:
         if (shift) {
            mStart = now;
            mState = DOWN_LISTENING;
            mTimeOut = now + mMsgDelta;
         }
         break;

      case DOWN_LISTENING:
         if (shift) {
            mEnd = now;
            mWord = (mEnd - mStart) / mMsgDelta;
            mState = WORD_READ;
         }
         break;

      default:
         mState = UP_LISTENING;
         break;
   }
   if (now >= mTimeOut) {
      mState = UP_LISTENING;
      mStart = 0;
      mEnd = 0;
   }
}
*/

/*
 * Something like a zero crossing:
 * return true if the delta between the last and current reading
 * of the pin is greater than the threshold the reader was set up with
 */
/*
bool Reader::detectShift () {
   mCurrentVal = analogRead(mPin);
   bool out;
  
   if (abs(mCurrentVal - mLastVal) >= mThreshold) {
      out = true;
      mLastVal = mCurrentVal;

      return out;
   }
   else {
      out = false;
      mLastVal = mCurrentVal;
      return out;
   }
}
*/
