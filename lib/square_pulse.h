/* 
 * square_pulse.h
 * Asynchronous square pulse generator for Arduino
 * Author:Danny Clarke
 */

#ifndef __SQUARE_PULSE_H__
#define __SQUARE_PULSE_H__

// MAIN DATA STRUCTURE
struct SquarePulse {
  double duration;
  double end_time;
  double next_write;
  double freq;
  int pin;
  int wave_state;
};

// FORWARD DECS
void setupSquarePulse(struct SquarePulse* sp, int pin,
                      double start_time, double dur, double freq);
void playSquarePulse (struct SquarePulse* sp);
void rescheduleSquarePulse (struct SquarePulse* sp, double dur);
void muteSquarePulse (struct SquarePulse* sp);
void setSquarePulseDur (struct SquarePulse* sp, double dur);
void setSquarePulseFreq (struct SquarePulse* sp, double freq);

// DEFINITIONS
void
setupSquarePulse(struct SquarePulse* sp, int pin,
                 double start_time, double dur, double freq)
{
  sp->next_write  = start_time * SECOND;
  sp->duration    = dur * SECOND;
  sp->end_time    = sp->duration + sp->next_write;
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

// Reschedule a square pulse DUR time in the future
//
void
rescheduleSquarePulse (struct SquarePulse* sp, double dur)
{
  double now = micros();
  if (now >= sp->end_time) {
    muteSquarePulse(sp);
    sp->next_write  = (dur * SECOND) + now;
    sp->end_time    = sp->next_write + sp->duration;
  }
}

// mute a square pulse
//
void
muteSquarePulse (struct SquarePulse* sp)
{
  digitalWrite(sp->pin, LOW);
}

// Set the duration, in seconds, of a square pulse
//
void
setSquarePulseDur (struct SquarePulse* sp, double dur)
{
  sp->duration = dur * SECOND;
}

// Set the frequency of a square pulse
//
void 
setSquarePulseFreq (struct SquarePulse* sp, double freq)
{
  sp->freq = freq;
}

#endif
