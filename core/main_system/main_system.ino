#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

const char* ssid     = "AGR";
const char* password = "mklf";
const char* host = "maker.ifttt.com";

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {32, 0, false};
Button button2 = {33, 0, false};
Button button3 = {34, 0, false};

long intterrupt_time = 0;
long t = 0;
volatile uint32_t deviceId = 10000;
void IRAM_ATTR isr_up() {
  intterrupt_time = millis();
  if ( intterrupt_time - t >  50 ) {
    button1.numberKeyPresses += 1;
    button1.pressed = true;
    deviceId++;
    t = millis();
  }
}

void IRAM_ATTR isr_down() {
  intterrupt_time = millis();
  if ( intterrupt_time - t >  50 ) {
    button2.numberKeyPresses += 1;
    button2.pressed = true;
    deviceId--;
    t = millis();
  }
}

void IRAM_ATTR isr_program() {
  intterrupt_time = millis();
  if ( intterrupt_time - t >  50 ) {
    button3.numberKeyPresses += 1;
    button3.pressed = true;
    t = millis();
  }
}

void setup() {
  byte count = 0;

  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(button2.PIN, INPUT_PULLUP);
  pinMode(button3.PIN, INPUT_PULLUP);

  //  attachInterruptArg(button1.PIN, isr, &button1, FALLING);
  attachInterrupt(button1.PIN, isr_up, FALLING);
  attachInterrupt(button2.PIN, isr_down, FALLING);
  attachInterrupt(button3.PIN, isr_program, FALLING);

}

void loop() {
  if (button1.pressed) {
    Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
    button1.pressed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button 1 ->" + String(button1.numberKeyPresses));
    lcd.setCursor(0, 1);
    lcd.print("DeviceID ->" + String(deviceId));
  }

  if (button2.pressed) {
    Serial.printf("Button 2 has been pressed %u times\n", button2.numberKeyPresses);
    button2.pressed = false;
    if (deviceId <= 9999) {
      deviceId = 10000;
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button 2 ->" + String(button2.numberKeyPresses));
    lcd.setCursor(0, 1);
    lcd.print("DeviceID ->" + String(deviceId));
  }

  if (button3.pressed) {
    Serial.printf("Button 3 has been pressed %u times\n", button2.numberKeyPresses);
    button3.pressed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Program Start");
  }

}
