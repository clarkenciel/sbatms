int piezo_pin = 7;
int led_pin   = 7;

int n_freqs   = 6;
float freqs[] = { 955, 1911, 1432, 1276, 1073, 758 };
double dur      = 0.0575;
double mil_val  = 1000.0;
double mic_val  = 1000.0;

void 
setup()
{
  pinMode(piezo_pin, OUTPUT);
  Serial.begin(9600);
}

void 
loop()
{
  double r = dur * mil_val;
  double d = r * mic_val;

  for (int i = 0; i < n_freqs; i++) {
    play_note(piezo_pin, d, freqs[i]);
    delay(r);
  }
}

void 
play_note (int pin, double dur, float freq)
{
  long start = 0.0;
  while (start < dur) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(freq / 2.0);

    digitalWrite(pin, LOW);
    delayMicroseconds(freq / 2.0);

    start += freq;
  }
  digitalWrite(pin, LOW);
}
