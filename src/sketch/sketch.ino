#include "pulse.h"

// TODO: Investigate different ways of generating form:
//        - Patterns of logic application
//        - Patterns of updating the wave state
//        - Convert sample rate into enum for easier switching

// pitch
int n_freqs = 6;
//float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
float freqs[] = { 0.1, 0.2, 0.3, 0.4 };
double next_freq;
int pitch_idx = 0;

struct SquarePulse sp;
struct SquarePulse sp2;
struct BitPulse bp;

double now;

void 
setup()
{
  Serial.begin(9600);
  setupBitPulse(&bp, 6, 1.0, 100.0);
  //setupBitPulse(&bp, 7, 1.0, 100.0);

  //setupSquarePulse(&sp, 6, 1.0, 1.0, freqs[0]);
  //setupSquarePulse(&sp2, 6, 0.0, 0.5, freqs[1]);
}

void 
loop()
{
  now = micros();
  playBitPulse(&bp);
  rescheduleBitPulse(&bp, 0.25);

  // playSquarePulse(&sp);
  // sp.freq = freqs[(pitch_idx = (pitch_idx + 1) % n_freqs)];
  // rescheduleSquarePulse(&sp, 0.25);

  // playing with a "second voice"
  // playSquarePulse(&sp2);

  // sp2.freq = freqs[(pitch_idx = (pitch_idx + 2) % n_freqs)];
  // rescheduleSquarePulse(&sp2, 0.33);
}

