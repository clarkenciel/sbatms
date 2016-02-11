#include "parser.h"
#include "Arduino.h"

Parser::Parser (const uint32_t * leader, uint16_t leaderSize, uint16_t messageSize)
  : mLeaderTemplate(leader)
  , mLeaderSize(leaderSize)
  , mMessageSize(messageSize)
  , mState(LEADER_LISTEN)
{
  // syntax with parens initializes to 0
  mLeader = new uint32_t[mLeaderSize]();
  mMessage = new uint32_t[mMessageSize]();
}

bool Parser::hasMessage () {
  switch (mState) {
  case MESSAGE_LISTEN:
    return true;
  default:
    return false;
  }
}

void Parser::parseMessage (uint32_t word) {
  if (word <= 10) {
    switch (mState) {
     // record in the leader buffer
     // if we match on the leader template, start recording messages
    case LEADER_LISTEN:
      mLeader[mLeaderWritePos++] = word;

      if (mLeaderWritePos == mLeaderSize) {
        if (leaderMatch())
          mState = MESSAGE_LISTEN;
        
        clearLeader();
        mLeaderWritePos = 0;
      }
      break;

      // record into the message buffer,
      // when full, transition to MESSAGE_READ
    case MESSAGE_LISTEN:
      mMessage[mMessageWritePos++] = word;
      if (mMessageWritePos == mMessageSize) {
        mState = MESSAGE_READ;
        mMessageWritePos = 0;
      }
      break;

    default:
      break;
    }
  }
}
      
bool Parser::leaderMatch () {
  uint16_t score = mLeaderSize;

  for (uint16_t i = 0; i < mLeaderSize; i++) {
    if (mLeaderTemplate[i] == mLeader[i])
      score--;
  }
  
  // convert int into it's bool equivalent, i.e. false if 0, true otherwise
  return !!mLeaderSize; 
}

void Parser::clearMessage () {
  for (uint16_t i = 0; i < mMessageSize; i++)
    mMessage[i] = 0;
}

void Parser::clearLeader () {
  for (uint16_t i = 0; i < mLeaderSize; i++)
    mLeader[i] = 0;
}

