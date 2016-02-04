#include "bit_pulse.h"
#include "reader.h"
#include "sender.h"

/*------------- AUDIO ------------- */
// TODO: Investigate different ways of generating form:
//        - Patterns of logic application
//        - Patterns of updating the wave state
//        - Convert sample rate into enum for easier switching


/* ------------- MESSAGES------------- */
// message outputs
#define leaderLen 3
#define msgLen 5

const uint32_t leader[leaderLen] = { 1, 2, 1 };
uint32_t coreMsg[msgLen];

// reading buffers
enum buf_state_t {
  LEADER_LISTEN = 0,
  MESSAGE_LISTEN,
  MESSAGE_READ,
  NUM_STATES
};

buf_state_t bState = LEADER_LISTEN;
uint16_t bufWritePtr = 0;
uint32_t inLdr[leaderLen];
uint32_t inMsg[msgLen];
void printBuf (uint16_t bufLen, const uint32_t * buf);
void clearBuf (uint16_t bufLen, uint32_t * buf);
bool bufEquals (uint16_t bOneLen, const uint32_t * bufOne, uint16_t bTwoLen, const uint32_t * bufTwo);
void parseMessage (Reader & reader);
void mutateCore ();

// reader and sender config
uint32_t now = 0;

int32_t xDelta = 2;
uint32_t msgDelta = 10000;

//Reader * readOne;
//Sender * sendOne;
//BitPulse * pulse;

Reader readOne = Reader(2, msgDelta, xDelta);
Sender sendOne = Sender(msgDelta, leaderLen, leader, msgLen, coreMsg);
BitPulse pulse = BitPulse(A0, msgLen, coreMsg);

// ------------------ PROGRAM ----------------------------
void 
setup()
{
  // start debug output
  Serial.begin(9600);
  Serial.println("Startup");

  // set up IR
  pinMode(3, OUTPUT);
  pinMode(2, INPUT);
  pinMode(A0, OUTPUT);

  // set up internal message
  Serial.print("Message: ");
  for (uint16_t i = 0; i < msgLen; i++)
  {
    coreMsg[i] = random(2, 10);
    Serial.print(coreMsg[i]);
    Serial.print(" ");
  }
  Serial.println();
  
  // set up reader, sender, and player
  //readOne = new Reader(2, msgDelta, xDelta);
  //sendOne = new Sender(msgDelta, leaderLen, leader, msgLen, coreMsg);
  //pulse   = new BitPulse(A0, msgLen, coreMsg);
}

void 
loop()
{
  now = micros();
  cli();

  // PLAY AUDIO
  //pulse->play(now);
  pulse.play(now);

  // MESSAGE HANDLING
  //sendOne->send2(now, &PORTD, B00001000); // set pin 3 to on
  //readOne->read2(now,  PIND,  B00000100); // read pin 2
  sendOne.send2(now, &PORTD, B00001000); // set pin 3 to on
  readOne.read2(now,  PIND,  B00000100); // read pin 2

  // store a leader or buffer
  parseMessage(readOne);

  // print message info
  if (bState == MESSAGE_READ)
  {
    mutateCore();

    Serial.print("INCOMING MESSAGE: ");
    printBuf(msgLen, inMsg);

    Serial.print("CORE MESSAGE: ");
    printBuf(msgLen, coreMsg);

    bState = LEADER_LISTEN;
    clearBuf(msgLen, inMsg);
  }
  sei();
}

void
mutateCore ()
{
  int32_t dif;
  int32_t cW, iW;
  for (uint16_t i = 0; i < msgLen; i++)
  {
    cW = coreMsg[i];
    iW = inMsg[i];
    dif = cW - iW;

    if (dif > 0)
      coreMsg[i] = constrain(cW - 1, 2, 10);
    else if (dif < 0)
      coreMsg[i] = constrain(cW + 1, 2, 10);
  }
}

// the following is here until I scope it to a class/struct
/* ------------- BUFFER HANDLING ------------- */
void
parseMessage (Reader & reader)
{
  if (reader.hasWord())
  {
    uint32_t w = reader.getWord();
    if (w <= 10)
    {
      switch (bState)
      {
        case LEADER_LISTEN:
          inLdr[bufWritePtr++] = w;
          if (bufWritePtr == leaderLen)
          {
            if (bufEquals(leaderLen, inLdr, leaderLen, leader))
            {
              bState = MESSAGE_LISTEN;

              Serial.print("LEADER: ");
              printBuf(leaderLen, inLdr);
            }
            clearBuf(leaderLen, inLdr);
            bufWritePtr = 0;
          }
          break;

        case MESSAGE_LISTEN:
          inMsg[bufWritePtr++] = w;
          if (bufWritePtr == msgLen)
          {
            bState = MESSAGE_READ;
            bufWritePtr = 0;

            Serial.print("INCOMING MESSAGE: ");
            printBuf(msgLen, inMsg);
          }
          break;

        default:
          break;
      }
    }
  }
}

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
