#ifndef __PULSE_H__
#define __PULSE_H__

#include "Arduino.h"

#define DUR_SCALE 1000.0 * 1000.0

struct SquarePulse {
  double attack_time;
  double duration;
  double end_time;
  double next_write;
  double freq;
  int pin;
  int wave_state;
};

void
setupSquarePulse(struct SquarePulse* sp, int pin,
                 double start_time, double dur, double freq)
{
  sp->attack_time = start_time * DUR_SCALE;
  sp->duration    = dur * DUR_SCALE;
  sp->end_time    = sp->duration + sp->attack_time;
  sp->next_write  = sp->attack_time;
  sp->freq        = freq;
  sp->wave_state  = 0;
  sp->pin         = pin;
  pinMode(pin, OUTPUT);
}

// generates a square wave using non-blocking scheduling
// rather than delay as above
//
void 
playSquarePulse (struct SquarePulse* sp)
{
  // if we have passed the next_write schedule,
  // write output to the pin
  // and schedule the next write
  double now = micros();

  if (now >= sp->next_write) {
    sp->wave_state = sp->wave_state ^ 1; // toggle wave state with XOR!

    switch (sp->wave_state) {
      case 1:
        //digitalWrite(sp->pin, HIGH);
        analogWrite(sp->pin, random(90,100));
        break;
      case 0:
        //digitalWrite(sp->pin, LOW);
        analogWrite(sp->pin, 0);
        break;
      default:
        break;
    }

    sp->next_write = now + sp->freq / 2.0;
  }
}

// Reschedule a square pulse
void
rescheduleSquarePulse (struct SquarePulse* sp, double dur)
{
  double now = micros();
  sp->attack_time = (dur * DUR_SCALE) + now;
  sp->end_time    = sp->attack_time + sp->duration;
  sp->next_write  = sp->attack_time;
}

// mute a square pulse
void
muteSquarePulse (struct SquarePulse* sp)
{
  digitalWrite(sp->pin, LOW);
}

void
setSquarePulseDur (struct SquarePulse* sp, double dur)
{
  sp->duration = dur * DUR_SCALE;
}

void setSquarePulseFreq (struct SquarePulse* sp, double freq)
{
  sp->freq = freq;
}
#endif

