
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup() {
  byte count = 0;

  lcd.begin();
  lcd.backlight();
  
  Serial.begin (115200);
  while (!Serial) {}
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");


  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (10);  // maybe unneeded?
      if (i == 63) {
        lcd.setCursor(0,0);
        lcd.print("LCD Founded");
        lcd.setCursor(0, 1);
        String msg = "Address : " + String(i);
        lcd.print(i);
      }
    } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}  // end of setup

void loop() {}
