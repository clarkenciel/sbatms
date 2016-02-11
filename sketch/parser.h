#pragma once

#include <stdint.h>

class Parser
{
 public:
  Parser (const uint32_t * leader, uint16_t leaderSize, uint16_t messageSize);
  ~Parser () {};

  void parseMessage (uint32_t word);
  bool hasMessage ();
  const uint32_t * getMessage () { return mMessage; }
  void listen () { mState = LEADER_LISTEN; clearMessage(); }
  
 private:
  enum parser_state_t
  {
    LEADER_LISTEN = 0,
    MESSAGE_LISTEN,
    MESSAGE_READ,
    NUM_STATES
  };

  uint32_t * mLeader;
  const uint32_t * mLeaderTemplate;
  uint32_t * mMessage;
  const uint16_t mLeaderSize, mMessageSize;
  uint16_t mLeaderWritePos, mMessageWritePos;
  parser_state_t mState;

  bool leaderMatch ();
  void clearMessage ();
  void clearLeader ();
};
