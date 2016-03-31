#pragma once

#include <stdint.h>

class Parser {
  public:
   
   // The leader pattern is set globally for a given box, so we pass by
   // const pointer here to save on copying, and to guarantee that
   // a Parser will not mutate that global leader pattern.
   Parser (const uint32_t * leader,
           uint16_t leaderSize,
           uint16_t messageSize,
           uint32_t timeout);
   ~Parser () {};

   void parseMessage (uint32_t word);
   bool hasMessage ();
   
   // returns const uint32_t* to prevent mutation of
   // internal message buffer by the caller.
   const uint32_t * getMessage () { return mMessage; }
   void listen () { mState = LEADER_LISTEN; clearMessage(); }
  
  private:
   enum parser_state_t {
      LEADER_LISTEN = 0,
      MESSAGE_LISTEN,
      MESSAGE_READ,
      NUM_STATES
   };

   uint32_t * mLeader;
   const uint32_t * mLeaderTemplate;
   uint32_t * mMessage;
   const uint16_t mLeaderSize, mMessageSize, mTimeout;
   uint16_t mLeaderWritePos, mMessageWritePos;
   parser_state_t mState;

   bool leaderMatch ();
   void clearMessage ();
   void clearLeader ();
};
