#include "bit_pulse.h"
#include "reader.h"
#include "sender.h"
#include "parser.h"

/* ------------- MESSAGES------------- */
// message outputs
#define leaderLen 3
#define msgLen 5

const uint32_t leader[leaderLen] = { 1, 2, 1 };
uint32_t coreMsg[msgLen];

// reader and sender config
uint32_t now = 0;

int32_t xDelta = 2;
uint32_t msgDelta = 10000;

Reader readOne = Reader(3, msgDelta, xDelta);
Reader readTwo = Reader(4, msgDelta, xDelta);
Reader readThree = Reader(5, msgDelta, xDelta);
Reader readFour = Reader(6, msgDelta, xDelta);

Parser parseOne = Parser(leader, leaderLen, msgLen);
Parser parseTwo = Parser(leader, leaderLen, msgLen);
Parser parseThree = Parser(leader, leaderLen, msgLen);
Parser parseFour = Parser(leader, leaderLen, msgLen);

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
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);

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
  sendOne.send2(now,  &PORTD, B00000100); // set pin 2 to on
  readOne.read2(now,   PIND,  B00001000); // read pin 3
  readTwo.read2(now,   PIND,  B00010000); // read pin 4
  readThree.read2(now, PIND,  B00100000); // read pin 5
  readFour.read2(now,  PIND,  B01000000); // read pin 6

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
  int32_t dif;
  int32_t cW, iW;
  for (uint16_t i = 0; i < msgLen; i++) {
    cW = coreMsg[i];
    iW = message[i];
    dif = cW - iW;

    if (dif > 0)
      coreMsg[i] = constrain(cW - 1, 1, 10);
    else if (dif < 0)
      coreMsg[i] = constrain(cW + 1, 1, 10);
  }
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
void randomizeCore () {
  for (uint16_t i = 0; i < msgLen; i++)
    coreMsg[i] = random(1,10);
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
