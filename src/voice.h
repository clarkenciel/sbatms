/*
 * voice.h
 * Author: Danny Clarke
 * November 26, 2015
 * 
 * Single "voice" of a given synth sound.
 * Attempts to be non-blocking by scheduling times for 
 * phase changes.
 *
 * Makes use of the GNU AVR libs:
 * http://www.nongnu.org/avr-libc/user-manual/modules.html
 *
 * Heavily indebted to Eric Heep and Adrian Freed:
 * https://github.com/clarkenciel/crystal-cube/blob/master/v2.0/lib/communication/sine/piezo_sine_waves/piezo_sine_waves.ino
 */

#ifndef __VOICE_H__
#define __VOICE_H__

#include "sine.h"
#include "avr/interrupt.h"
#include "avr/pgmspace.h"

// as we build out more wave types, increase this enum
enum wavetype {
  SINE = 0
};

typedef struct _Voice {
  int phase_index; // current phase index
  double freq; // frequency
  double phase; // current phase
  long next_phase_change; // next time to calc phase index
  wavetype table;
} voice;

/*
 * inc_phase (voice v)
 *
 * 1. Check current time.
 * 2. If it matches the scheduled time for phase update,
 *    update phase.
 */
void 
inc_phase (voice v)
{
}

/*
 * table_lookup (wavetype w, int index)
 *
 * Fetch value in the wavetable indicated by wavetable w.
 */
char
table_lookup (wavetype w, int index)
{
  switch (w)
  {
    case SINE:
      return sinetable[index];
  }
}

/*
 * get_val (voice v)
 *
 * 1. Check the voice's wave type
 * 2. Look up value in corresponding table according
 *    to wave type and current phase index.
 */
int 
get_val (voice v)
{
  return table_lookup(v->table, v->phase_index);
}

#endif
