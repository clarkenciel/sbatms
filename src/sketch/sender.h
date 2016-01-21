#pragma once
#include <stdint.h>

class Sender
{
  enum sender_state_t 
  {
    LEADER = 0,
    MESSAGE,
    NUM_SND_STATES,
  };

  enum message_state_t 
  {
    DOWN = 0,
    UP,
    NUM_MSG_STATES,
  };
  
  public:
    Sender (uint32_t delta, uint16_t leaderLen, const uint32_t * leader, uint16_t msgLen, const uint32_t * msg);
    ~Sender () {};

    void send (uint32_t now, uint16_t numPins, const uint16_t * pins);
    void send2 (uint32_t now, volatile uint8_t * pinRegister, uint8_t pinTargets);

  private:
    const uint32_t * mMsg;
    const uint32_t * mLeader;
    
    sender_state_t mSndState;
    message_state_t mMsgState;
    
    uint32_t mNextWriteTime, mMsgDelta;
    uint16_t mWritePointer, mLeaderLen, mMsgLen;

    void sendOut ( uint16_t pin);
    void scheduleNextWrite(uint32_t now);
    void sendOut2 (volatile uint8_t * pinRegister, uint8_t pinTargets);
    void changePhase ();
};


