#include "reader.h"  
#include "Arduino.h"

Reader::Reader (uint16_t p, uint32_t md, int32_t cd)
  : mPin(p)
  , mMsgDelta(md)
  , mThreshold(cd)
  , mCurrentVal(cd)
  , mLastVal(cd)
{
  mState = UP_LISTENING;  
}

/*
 * Words correspond to the length of time
 * between intensity changes in the incoming signal (see detectShift).
 * This will record those words using an input time
 */
void
Reader::read (uint32_t now)
{
  bool shift = detectShift();
  switch (mState)
  {
    case UP_LISTENING:
      if (shift)
      {
        mStart = now;
        mState = DOWN_LISTENING;
      }
      break;
    case DOWN_LISTENING:
      if (shift)
      {
        mEnd = now;
        mWord = (mEnd - mStart) / mMsgDelta;
        mState = WORD_READ;
      }
      break;
    default:
      mState = UP_LISTENING;
      break;
  }
}

void
Reader::read2 (uint32_t now, const uint8_t pinRegister, const uint8_t pins)
{
  bool shift = detectShift2(pinRegister, pins);
  switch (mState)
  {
    case UP_LISTENING:
      if (shift)
      {
        mStart = now;
        mState = DOWN_LISTENING;
      }
      break;
    case DOWN_LISTENING:
      if (shift)
      {
        mEnd = now;
        mWord = (mEnd - mStart) / mMsgDelta;//(int) (((float) (mEnd - mStart)) / ((float) mMsgDelta));
        if (mWord > 0)
          mState = WORD_READ;
      }
      break;
    default:
      mState = UP_LISTENING;
      break;
  }
}

/*
 * Verify whether the reader has read a word
 * that has not been taken already
 */
bool
Reader::hasWord ()
{
  switch(mState)
  {
    case WORD_READ:
      return true;
    default:
      return false;
  }
}

/*
 * Return the word and mark the reader as read
 */
uint32_t
Reader::getWord ()
{
  mState = WORD_TAKEN;
  return mWord;
}

/*
 * Something like a zero crossing:
 * return true if the delta between the last and current reading
 * of the pin is greater than the threshold the reader was set up with
 */
bool
Reader::detectShift ()
{
  mCurrentVal = analogRead(mPin);
  bool out;
  
  if (abs(mCurrentVal - mLastVal) >= mThreshold)
  {
    
    //Serial.println(mCurrentVal);
    out = true;
    mLastVal = mCurrentVal;

    return out;
  }
  else
  {
    out = false;
    mLastVal = mCurrentVal;
    return out;
  }
}

bool
Reader::detectShift2 (const uint8_t pinRegister, const uint8_t pins)
{
  mCurrentVal = pinRegister & pins;
//  Serial.print(mLastVal, BIN);
//  Serial.print(", ");
//  Serial.println(mCurrentVal, BIN);

  // lo -> hi || hi -> lo
  if ((mCurrentVal && !mLastVal) || (!mCurrentVal && mLastVal))
  {
    mLastVal = mCurrentVal;
    return true;
  }
  else
  {
    mLastVal = mCurrentVal;
    return false;
  }
}




