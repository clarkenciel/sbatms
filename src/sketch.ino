int piezo_pin = 6;

// pitch
int n_freqs   = 6;
float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
int pitch_idx = 0;

// rhythm
double dur      = 0.0500;
double mil_val  = 1000.0;
double mic_val  = 1000.0;

// writing vars
int wave_state = 1;
double rhythm_pulse = dur * mil_val * mic_val;
double duration_amt = rhythm_pulse;
double next_write   = rhythm_pulse;
double pulse_start = rhythm_pulse;
double pulse_end   = pulse_start + duration_amt;

void 
setup()
{
  pinMode(piezo_pin, OUTPUT);
  digitalWrite(piezo_pin, LOW);
  Serial.begin(9600);
}

void 
loop()
{
  double now = micros();

  // play a tone if we are past the scheduled
  // pulse start time
  if (now >= pulse_start) {
    nb_square(piezo_pin, freqs[pitch_idx]);
  } 

  // schedule the next pulse_start/end time if we are past
  // the scheduled end time
  if (now >= pulse_end) {
    pulse_start = rhythm_pulse + now;
    pulse_end   = pulse_start + duration_amt;
    pitch_idx   = (pitch_idx + 1) % n_freqs;
    nb_square(piezo_pin, 0);
  }
}

// generates a square wave using non-blocking scheduling
// rather than delay as above
//
void 
nb_square (int pin, float freq)
{
  // if we have passed the next_write schedule,
  // write output to the pin
  // and schedule the next write
  double now = micros();

  if (now >= next_write) {
    wave_state = wave_state ^ 1; // toggle wave state with XOR!

    switch (wave_state) {
      case 1:
        digitalWrite(pin, HIGH);
        break;
      case 0:
        digitalWrite(pin, LOW);
        break;
      default:
        break;
    }

    next_write = now + freq / 2.0;
  }
}
  
      

