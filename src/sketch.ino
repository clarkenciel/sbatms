#include "../lib/pulse.h"

// pitch
int n_freqs   = 6;
float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
int pitch_idx = 0;

struct SquarePulse sp;

void 
setup()
{
  setupSquarePulse(&sp, 6, 0, 0.5, 900);
}

void 
loop()
{
  double now = micros();

  if (now >= sp.attack_time) {
    playSquarePulse(&sp);
  }

  if (now >= sp.end_time) {
    muteSquarePulse(&sp);
    sp.freq = freqs[(pitch_idx = (pitch_idx + 1) % n_freqs)];
    rescheduleSquarePulse(&sp, 0.25);
  }
}

