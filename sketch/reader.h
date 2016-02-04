#pragma once
#include <stdint.h>

class Reader 
{
  enum reader_state_t
  {
    UP_LISTENING = 0,
    DOWN_LISTENING,
    WORD_READ,
    WORD_TAKEN,
    NUM_STATES
  };
  
  public:
    Reader (uint16_t pin, uint32_t msgDelta, int32_t crossDelta);
    ~Reader () {};

    void read(uint32_t now);
    void read2 (uint32_t now, uint8_t pinRegister, uint8_t pins);
    bool hasWord ();
    uint32_t getWord ();
    
  private:
    uint16_t mPin;
    uint32_t mStart, mEnd, mWord, mMsgDelta, mTimeOut;
    int32_t mThreshold, mLastVal, mCurrentVal;
    reader_state_t mState;

    bool detectShift ();
    bool detectShift2 (uint8_t pinRegister, uint8_t pins);
};

