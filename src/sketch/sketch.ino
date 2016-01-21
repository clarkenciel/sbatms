#include "pulse.h"
#include "reader.h"
#include "sender.h"

/*------------- AUDIO ------------- */
// TODO: Investigate different ways of generating form:
//        - Patterns of logic application
//        - Patterns of updating the wave state
//        - Convert sample rate into enum for easier switching

// pitch
double next_freq;
float freqs[]      = { 0.1, 0.2, 0.3, 0.4 };
uint16_t n_freqs   = 6;
uint16_t pitch_idx = 0;

struct SquarePulse sp;
struct SquarePulse sp2;
struct BitPulse bp;

/* ------------- MESSAGES------------- */
// message outputs
#define leaderLen 3
#define numOutPins 1
#define msgLen 5

const uint32_t leader[leaderLen] = { 1, 2, 1 };
const uint32_t msg[msgLen] = { 2, 3, 3, 2, 2 };

// reading buffers
enum buf_state_t {
  LEADER = 0,
  MESSAGE,
  NUM_STATES
};

buf_state_t bState = LEADER;
uint16_t bufWritePtr = 0;
uint32_t lBuf[leaderLen];
uint32_t mBuf[msgLen];
void printBuf (uint16_t bufLen, const uint32_t * buf);
void clearBuf (uint16_t bufLen, uint32_t * buf);
bool bufEquals (uint16_t bOneLen, const uint32_t * bufOne, uint16_t bTwoLen, const uint32_t * bufTwo);

// reader and sender config
uint32_t now = 0;
uint16_t outPins[numOutPins] = {3};

int32_t xDelta = 2;
uint32_t msgDelta = 1000;

Reader readOne = Reader(2, msgDelta, xDelta);
Sender sendOne = Sender(msgDelta, leaderLen, leader, msgLen, msg);

// ------------------ PROGRAM ----------------------------
void 
setup()
{
  // start debug output
  Serial.begin(9600);
  Serial.println("Startup");

  // set up audio
  setupBitPulse(&bp, 4, 1.0, 100.0);

  // set up IR
  pinMode(3, OUTPUT);
  pinMode(2, INPUT);
}

void 
loop()
{
  cli();
  now = micros();

  // PLAY AUDIO
  playBitPulse(&bp, now);
  rescheduleBitPulse(&bp, now, 0.25);

  // MESSAGE HANDLING
  sendOne.send2(now, &PORTD, B00001000); // set pin 3 to on
  readOne.read2(now,  PIND,  B00000100); // read pin 2

  // store a leader or buffer
  if (readOne.hasWord())
  {
    uint32_t w = readOne.getWord();
    if (w < 5)
    {
      switch (bState)
      {
        case LEADER:
          lBuf[bufWritePtr++] = w;
          if (bufWritePtr == leaderLen)
          {
            
            if (bufEquals(leaderLen, lBuf, leaderLen, leader))
            {
              //Serial.print("LEADER: ");
              //printBuf(leaderLen, lBuf);
              bState = MESSAGE;
            }
            clearBuf(leaderLen, lBuf);
            bufWritePtr = 0;
          }
          break;
        case MESSAGE:
          mBuf[bufWritePtr++] = w;
          if (bufWritePtr == msgLen)
          {
            bState = LEADER;
            bufWritePtr = 0;
            //Serial.print("MESSAGE: ");
            //printBuf(msgLen, mBuf);
            clearBuf(msgLen, mBuf);
          }
          break;
      }
    }
  }
  sei();
}

// the following is here until I scope it to a class/struct
/* ------------- BUFFER HANDLING ------------- */
void
printBuf (uint16_t bufLen, const uint32_t * buf)
{
  Serial.print("[");
  for (uint16_t i = 0; i < bufLen; i++)
  {
    Serial.print(buf[i]);
    Serial.print(" ");
  }
  Serial.println("]");
}

void
clearBuf (uint16_t bufLen, uint32_t * buf)
{
  for (uint16_t i = 0; i < bufLen; i++)
    buf[i] = 0;
}

bool
bufEquals (uint16_t bOneLen, const uint32_t * bufOne, uint16_t bTwoLen, const uint32_t * bufTwo)
{
  if (bOneLen != bTwoLen)
    return false;
  else
  {
    for (uint16_t i; i < bOneLen; i++)
    {
      if (bufOne[i] != bufTwo[i])
        return false;
    }
    return true;
  }
}
