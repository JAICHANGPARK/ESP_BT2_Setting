#include <SoftwareSerial.h>

SoftwareSerial btSerial(2, 3);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  btSerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
//  for (int i = 0 ; i < 360; i++) btSerial.println(sin(DEG_TO_RAD * i));
  int buffCount = 0;
  uint8_t buff[] = {};
  
  buff[buffCount++] = 'R';
  buff[buffCount++] = 'B';
  buff[buffCount++] = 'P';
  buff[buffCount++] = ':';
  buff[buffCount++] = 0x72;
  buff[buffCount++] = 0x11;
  buff[buffCount++] = 0x10;
  buff[buffCount++] = 0x10;
  buff[buffCount++] = 0x10;
  uint16_t userStep = 1350;
  buff[buffCount++] = (uint8_t)(userStep >> 8) & 0xFF ;
  buff[buffCount++] = (uint8_t)(userStep) & 0xFF ;
  buff[buffCount++] = 0x10;
  buff[buffCount++] = 0x41;

  for(int i = 0; i < buffCount; i++){
     btSerial.print(buff[i]);
  }
  
  btSerial.println();
  buffCount = 0;
  
  if (btSerial.available())
  {
    delay(5);
    while (btSerial.available())
    {
      Serial.write(btSerial.read());
    }
  }
  delay(200);
}
