#include "../lib/pulse.h"

// pitch
int n_freqs   = 6;
//float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
float freqs[] = { 0.1, 0.2, 0.3, 0.4 };
int pitch_idx = 0;

struct SquarePulse sp;
struct SquarePulse sp2;

void 
setup()
{
  setupSquarePulse(&sp, 6, 1.0, 1.0, freqs[0]);
  //setupSquarePulse(&sp2, 6, 0.0, 0.5, freqs[1]);
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

  // playing with a "second voice"
  // if (now >= sp2.attack_time) {
  //   playSquarePulse(&sp2);
  // }

  // if (now >= sp2.end_time) {
  //   muteSquarePulse(&sp2);
  //   sp2.freq = freqs[(pitch_idx = (pitch_idx + 2) % n_freqs)];
  //   rescheduleSquarePulse(&sp2, 0.33);
  // }
}

