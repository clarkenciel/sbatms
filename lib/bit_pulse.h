/*
 * bit_pulse.h
 * 
 * Bit logic based synthesis per the work of Viznut:
 * http://countercomplex.blogspot.com/2011/10/algorithmic-symphonies-from-one-line-of.html
 *
 * Author: Danny Clarke
 */

#ifndef __BIT_PULSE_H__
#define __BIT_PULSE_H__

#include "math.h"

// MAIN DATA STRUCT
struct BitPulse {
  double next_write;
  double duration;
  double end_time;

  int pin;
  unsigned long wave_state;
};

// FORWARD DECS
void setupBitPulse(struct BitPulse* sp, int pin,
                 double start_time, double dur, double freq);
void playBitPulse (struct BitPulse* sp);
void rescheduleBitPulse (struct BitPulse* sp, double dur);
void muteBitPulse (struct BitPulse* sp);
void setBitPulseDur (struct BitPulse* sp, double dur);

// DEFINITIONS
void
setupBitPulse(struct BitPulse* sp, int pin,
                 double start_time, double dur)
{
  sp->next_write  = start_time * SECOND;
  sp->duration    = dur * SECOND;
  sp->end_time    = sp->duration + sp->next_write;
  sp->wave_state  = 0; // init to one to avoid zero-only state
  sp->pin         = pin;
  pinMode(pin, OUTPUT);
}

// Assemble "words" into a sentence
void 
playBitPulse (struct BitPulse* sp)
{
  double now = micros();
  double out;

  if (now >= sp->next_write) {
    sp->wave_state++;
    out = ((sin(sp->wave_state >> 1) + cos(sp->wave_state >> 2)) 
        + (sp->wave_state >> (sp->wave_state | 100))) * 10;
    analogWrite(sp->pin, out); 
    sp->next_write = now + SAMPLE;
  }
}

// Reschedule a square pulse DUR time in the future
//
void
rescheduleBitPulse (struct BitPulse* sp, double dur)
{
  double now = micros();
  if (now >= sp->end_time) {
    muteBitPulse(sp);
    sp->next_write  = (dur * SECOND) + now;
    sp->end_time    = sp->next_write + sp->duration;
  }
}

// mute a square pulse
//
void
muteBitPulse (struct BitPulse* sp)
{
  digitalWrite(sp->pin, LOW);
}

// Set the duration, in seconds, of a square pulse
//
void
setBitPulseDur (struct BitPulse* sp, double dur)
{
  sp->duration = dur * SECOND;
}

#endif
