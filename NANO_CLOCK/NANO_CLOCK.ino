// generate a MCKL signal pin
 const int clock = 9;
void setup() {
  pinMode(clock, OUTPUT);

}

void loop() {
  TCCR1B = (TCCR1B & 0xF8) | 1 ; //generates the MCKL signal
  analogWrite (clock, 128) ; 

}
