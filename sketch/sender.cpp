#include "sender.h"
#include "Arduino.h"

Sender::Sender (uint32_t delta
              , uint16_t leaderLen
              , const uint32_t * leader
              , uint16_t msgLen
              , const uint32_t * msg)
  : mMsgDelta(delta)
  , mLeader(leader)
  , mMsg(msg)
  , mLeaderLen(leaderLen)
  , mMsgLen(msgLen)
  , mNextWriteTime(0)
  , mMsgState(DOWN)
  , mSndState(LEADER)
  , mWritePointer(0)
{
}

void
Sender::send (uint32_t now, uint16_t numPins, const uint16_t * pins)
{
  if (now >= mNextWriteTime)
  {
//    Serial.print(mSndState); Serial.print(" -> ");
//    Serial.print(mMsgState); Serial.print(": ");
    for (uint16_t i; i < numPins; i++)
      sendOut(pins[i]);
    scheduleNextWrite(now);
    changePhase();
  }
}

void
Sender::send2 (uint32_t now, volatile uint8_t * pinRegister, uint8_t pinTargets)
{
  if (now >= mNextWriteTime)
  {
//    Serial.print(mSndState); Serial.print(" -> ");
//    Serial.print(mMsgState); Serial.print(": ");
    sendOut2(pinRegister, pinTargets);
    scheduleNextWrite(now);
    changePhase();
  }
}


void
Sender::sendOut (uint16_t pin)
{
//  Serial.print(PORTD, BIN); Serial.print(", ");
  switch (mMsgState)
  {
    case DOWN:
      digitalWrite(pin, LOW);
      break;
    case UP:
      digitalWrite(pin, HIGH);
      break;
  }
//  Serial.println(PORTD,BIN);
}

void
Sender::sendOut2 (volatile uint8_t * pinRegister, uint8_t pinTargets)
{
  //Serial.print(PORTD, BIN); Serial.print(", ");
  switch (mMsgState)
  {
    case DOWN:
      *pinRegister &= 0;
      break;
    case UP:
      *pinRegister |= pinTargets;
      break;
  }
  //Serial.println(PORTD, BIN);
}

void
Sender::scheduleNextWrite(uint32_t now)
{
  uint32_t interval = 0;

  if (mMsgState == DOWN)
    mNextWriteTime = now + mMsgDelta;
  else
  {
    switch (mSndState)
    {
      case LEADER:
        interval = mLeader[mWritePointer++];
        break;
      case MESSAGE:
        interval = mMsg[mWritePointer++];
        break;
    }
  
    //Serial.println(interval);
    mNextWriteTime = now + interval*mMsgDelta;
  }
}


void
Sender::changePhase ()
{
  if (mMsgState == DOWN)
    mMsgState = UP;
  else 
    mMsgState = DOWN;
  switch (mSndState)
  {
    case LEADER:
      if (mWritePointer == mLeaderLen)
      {
        mWritePointer = 0;
        mSndState = MESSAGE;
        mMsgState = DOWN;
      }
      break;
      
    case MESSAGE:
      if (mWritePointer == mMsgLen)
      {
        mWritePointer = 0;
        mSndState = LEADER;
        mMsgState = DOWN;
      }
      break;
  }
}

