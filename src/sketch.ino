#include "../lib/pulse.h"

// pitch
int n_freqs = 6;
//float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
float freqs[] = { 0.1, 0.2, 0.3, 0.4 };
double next_freq;
int pitch_idx = 0;

struct SquarePulse sp;
struct SquarePulse sp2;

double now;

void 
setup()
{
  setupSquarePulse(&sp, 6, 1.0, 1.0, freqs[0]);
  //setupSquarePulse(&sp2, 6, 0.0, 0.5, freqs[1]);
}

void 
loop()
{
  now = micros();
  playSquarePulse(&sp);
  if (now >= next_freq) 
  sp.freq = freqs[(pitch_idx = (pitch_idx + 1) % n_freqs)];
  rescheduleSquarePulse(&sp, 0.25);

  // playing with a "second voice"
  // playSquarePulse(&sp2);

  // sp2.freq = freqs[(pitch_idx = (pitch_idx + 2) % n_freqs)];
  // rescheduleSquarePulse(&sp2, 0.33);
}

