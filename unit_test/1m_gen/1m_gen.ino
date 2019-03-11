const uint8_t OUTPUT_PIN = 3;  // = OC2B
const uint8_t PERIOD = 8;      // 9 CPU cycles ~ 1.778 MHz

void setup()
{
  pinMode(OUTPUT_PIN, OUTPUT);
  TCCR2B = 0;           // stop timer
  TCNT2  = 0;           // reset timer
  TCCR2A = _BV(COM2B1)  // non-inverting PWM on OC2B
           | _BV(WGM20)   // fast PWM mode, TOP = OCR2A
           | _BV(WGM21);
  TCCR2B = _BV(WGM22);
  OCR2A = PERIOD - 1 ;
  OCR2B = PERIOD / 2 - 1;
}


void loop() {
  // put your main code here, to run repeatedly:
  TCCR2B &= ~_BV(CS20);
  TCNT2 = 0;
  delay(5000);
  TCCR2B |= _BV(CS20);
  delay(5000);
  TCCR2B &= ~_BV(CS20);
  TCNT2 = 0;
  delay(5000);
}
