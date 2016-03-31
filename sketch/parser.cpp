#include "parser.h"
#include "Arduino.h"

Parser::Parser (const uint32_t * leader,
                uint16_t leaderSize,
                uint16_t messageSize,
                uint32_t timeout)
  : mLeaderTemplate(leader)
  , mLeaderSize(leaderSize)
  , mMessageSize(messageSize)
  , mTimeout(timeout)
  , mState(LEADER_LISTEN) {
  // syntax with parens initializes to 0
  mLeader = new uint32_t[mLeaderSize]();
  mMessage = new uint32_t[mMessageSize]();
}

/*
 * Return true if the parser has completed parsing from a
 * Reader.
 */
bool Parser::hasMessage () {
  switch (mState) {
  case MESSAGE_READ:
    return true;
  default:
    return false;
  }
}

/*
 * Accepts "words" (durations a Reader spends in UP state)
 * and records them as part of the leader to a message, or as
 * the body to a message, depending on state.
 *
 * State will transition from LEADER_LISTEN to MESSAGE_LISTEN
 * when the Parser receives a number of words equal to the length
 * of a leader and matching the leader pattern that it is looking for.
 *
 * State will transition from MESSAGE_LISTEN to MESSAGE_READ when
 * the Parser receives a number of words equal to the length of a message.
 *
 * If the Parser is in a MESSAGE_READ state when this method is called,
 * the Parser will simply pass through.
 */
void Parser::parseMessage (uint32_t word) {
  // a kind of timeout: words > 10 are invalid
  if (word <= mTimeout) { 
    switch (mState) {

      // record in the leader buffer
      // if we match on the leader template, start recording messages
    case LEADER_LISTEN:
      // set and increment
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
      
/*
 * Check if the Parser's current buffer of words matches the
 * leader pattern it is looking for.
 */
bool Parser::leaderMatch () {
  uint16_t score = mLeaderSize;

  for (uint16_t i = 0; i < mLeaderSize; i++) {
    if (mLeaderTemplate[i] == mLeader[i])
      score--;
  }
  
  // if score == 0, then we have a match (!0 == true), otherwise false
  return !mLeaderSize; 
}

/*
 * Reset a the Parser's message buffer to 0s
 */
void Parser::clearMessage () {
  for (uint16_t i = 0; i < mMessageSize; i++)
    mMessage[i] = 0;
}

/*
 * Reset the Parser's leader buffer to 0s
 */
void Parser::clearLeader () {
  for (uint16_t i = 0; i < mLeaderSize; i++)
    mLeader[i] = 0;
}

