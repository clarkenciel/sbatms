#pragma once
#include <stdint.h>

/*
 * State machine for recording, as "words" the length of time
 * that another box's LED is turned on.
 */
class Reader {

   enum reader_state_t {
      UP_LISTENING = 0,
      DOWN_LISTENING,
      WORD_READ,
      WORD_TAKEN,
      NUM_STATES
   };
  
  public: 
   Reader (uint16_t pin, uint32_t msgDelta, uint32_t timeOutInterval);
   ~Reader () {};

   void read2 (uint32_t now, uint8_t pinRegister, uint8_t pins);
   bool hasWord ();
   uint32_t getWord ();

   /* DEPRECATED: void read(uint32_t now); */
   /* DEPRECATED: Reader (uint16_t pin, uint32_t msgDelta, int32_t crossDelta); */
    
  private:
   uint16_t mPin;
   uint32_t mStart, mEnd, mWord, mMsgDelta;
   uint32_t mTimeOut, mTimeOutInterval;
   int32_t  mLastVal, mCurrentVal;
   reader_state_t mState;

   bool detectShift2 (uint8_t pinRegister, uint8_t pins);

   /* DEPRECATED: bool detectShift (); */
   /* DEPRECATED: int32_t mThreshold; */
};

