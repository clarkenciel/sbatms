#include "bit_pulse.h"
#include "reader.h"
#include "sender.h"
#include "parser.h"

/* ------------- MESSAGES------------- */
// message outputs
const uint16_t leaderLen = 3;
const uint16_t msgLen = 5;

const uint32_t leader[leaderLen] = { 1, 2, 1 };
uint32_t coreMsg[msgLen];

// reader and sender config
uint32_t now = 0;

uint32_t msgTimeoutDelta = 11;
uint32_t msgDelta = 10000;
uint32_t timeout = msgTimeoutDelta * msgDelta;

Reader readOne = Reader(msgDelta, timeout);
Reader readTwo = Reader(msgDelta, timeout);
Reader readThree = Reader(msgDelta, timeout);
Reader readFour = Reader(msgDelta, timeout);

Parser parseOne = Parser(leader, leaderLen, msgLen, timeout);
Parser parseTwo = Parser(leader, leaderLen, msgLen, timeout);
Parser parseThree = Parser(leader, leaderLen, msgLen, timeout);
Parser parseFour = Parser(leader, leaderLen, msgLen, timeout);

Sender sendOne = Sender(msgDelta, leaderLen, leader, msgLen, coreMsg);
BitPulse pulse = BitPulse(A0, msgLen, coreMsg);

// reading buffers and core message handling
void printBuf (uint16_t bufLen, const uint32_t * buf);
void mutateCore (const uint32_t * message);
bool coreFlatlined ();
void randomizeCore ();

// ------------------ PROGRAM ----------------------------
void setup () {
  // start debug output
  Serial.begin(9600);
  Serial.println("Startup");

  // set up IR
  // 2 - 6 are outputs (this is for variation in my perf-board circuits)
  for (int i = 2; i < 7; i++)
      pinMode(i, OUTPUT);

  // 8 - 11 are inputs
  for (int i = 8; i < 12; i++)
      pinMode(i, INPUT);

  // set up audio
  pinMode(A0, OUTPUT);

  // set up internal message
  Serial.print("Message: ");
  for (uint16_t i = 0; i < msgLen; i++) {
    coreMsg[i] = random(2, 10);
    Serial.print(coreMsg[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  now = micros();
  cli();

  // PLAY AUDIO
  pulse.play(now);
 
  // MESSAGE HANDLING
  sendOne.send2(now,  &PORTD, B11111100); // set pin 2 to on
  readOne.read2(now,   PINB,  B00000010); // read pin 9
  readTwo.read2(now,   PINB,  B00000100); // read pin 10
  readThree.read2(now, PINB,  B00001000); // read pin 11
  readFour.read2(now,  PINB,  B00010000); // read pin 12

  // store a leader or buffer
  if (readOne.hasWord()) {
    parseOne.parseMessage(readOne.getWord());
  }
  if (readTwo.hasWord()) {
    parseOne.parseMessage(readTwo.getWord());
  }
  if (readThree.hasWord()) {
    parseOne.parseMessage(readThree.getWord());
  }
  if (readFour.hasWord()) {
    parseOne.parseMessage(readFour.getWord());
  }

  // mutate core, maybe
  if (parseOne.hasMessage()) {
    mutateCore(parseOne.getMessage());
    Serial.println("message one!");
    printBuf(msgLen, coreMsg);
    parseOne.listen();
  }
  if (parseTwo.hasMessage()) {
    mutateCore(parseTwo.getMessage());
    Serial.println("message two!");
    printBuf(msgLen, coreMsg);
    parseTwo.listen();
  }
  if (parseThree.hasMessage()) {
    mutateCore(parseThree.getMessage());
    Serial.println("message three!");
    printBuf(msgLen, coreMsg);
    parseThree.listen();
  }
  if (parseFour.hasMessage()) {
    mutateCore(parseFour.getMessage());
    Serial.println("message four!");
    printBuf(msgLen, coreMsg);
    parseFour.listen();
  }
  if (coreFlatlined())
    randomizeCore();

  sei();
}

// mutate the values in the core message so that they
// converge on another message
void mutateCore (const uint32_t * message) {
  uint16_t idx = random(0, msgLen);
  int32_t cW = coreMsg[idx], mW = message[idx];
  int32_t dif = cW - mW;
  if (dif > 0) coreMsg[idx] = constrain(cW - 1, 1, 10);
  else if (dif < 0) coreMsg[idx] = constrain(cW + 1, 1, 10);
  //coreMsg[idx] = message[idx];
//  int32_t dif;
//  int32_t cW, iW;
//  for (uint16_t i = 0; i < msgLen; i++) {
//    cW = coreMsg[i];
//    iW = message[i];
//    dif = cW - iW;
//
//    if (dif > 0)
//      coreMsg[i] = constrain(cW - 1, 1, 10);
//    else if (dif < 0)
//      coreMsg[i] = constrain(cW + 1, 1, 10);
//  }
}

// print out a buffer of 32-bit unsigned int values
void printBuf (uint16_t bufLen, const uint32_t * buf) {
  Serial.print("[");
  for (uint16_t i = 0; i < bufLen; i++) {
    Serial.print(buf[i]);
    Serial.print(" ");
  }
  Serial.println("]");
}

// randomize the values in the core message
// IDEA: change "personality" - likelihood of change
void randomizeCore () {
  for (uint16_t i = 0; i < msgLen; i++)
    coreMsg[i] = random(1, 10);
}

// check whether the core message has become
// exclusively one value
bool coreFlatlined () {
  uint16_t i = 0;
  uint32_t lastVal = coreMsg[0];
  bool flat = true;

  while (i < msgLen && flat) {
    flat = flat && (lastVal == coreMsg[i]);
    lastVal = coreMsg[i++];
  }

  return flat;
}
