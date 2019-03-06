
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>


#define AP_NAME       "Himart-WiFi"
#define IFTTT_HOST    "maker.ifttt.com"
#define IFTTT_KEY     "dvb-A7THFIQYAwGUmP8Yck"

#define MCU_RX2       16
#define MCU_TX2       17

#define BLUETOOTH_BAUDRATE          38400

const char* ssid     = "";
const char* password = "";
const char* host = "maker.ifttt.com";


long intterrupt_time = 0;
long t = 0;
volatile uint32_t deviceId = 10000;

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {32, 0, false};
Button button2 = {33, 0, false};
Button button3 = {34, 0, false};
LiquidCrystal_I2C lcd(0x3f, 16, 2);
HardwareSerial bluetoothSerial(2);




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

void send_event(const char *event) {
  Serial.print("Connecting to");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += IFTTT_KEY;
  String mJson = String("{\"value1\" : \"") + "Test\", " +
                 String("\"value2\" : \"") + "Test\", " +
                 String("\"value3\" : \"") + "Test\"} ";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  String reqHeader = "POST " + url + " HTTP/1.1\r\n";
  String reqHost = "HOST: " + String(IFTTT_HOST) + "\r\n";
  String reqType = "Content-Type: application/json\r\n";
  String reqLength = "Content-Length: " + String(mJson.length()) + "\r\n";


  client.print(reqHeader);
  client.print(reqHost);
  client.print(reqType);
  client.print(reqLength);
  client.println();
  client.println(mJson);

  unsigned long timeout = millis();

  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.connected()) {
    // Read all the lines of the reply from server and print them to Serial
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      delay(50);
    }
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

void send_event(const char *event, String type, String address, String baud) {
  Serial.print("Connecting to");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += IFTTT_KEY;
  String mJson = String("{\"value1\" : \"") + "Test\", " +
                 String("\"value2\" : \"") + "Test\", " +
                 String("\"value3\" : \"") + "Test\"} ";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  String reqHeader = "POST " + url + " HTTP/1.1\r\n";
  String reqHost = "HOST: " + String(IFTTT_HOST) + "\r\n";
  String reqType = "Content-Type: application/json\r\n";
  String reqLength = "Content-Length: " + String(mJson.length()) + "\r\n";


  client.print(reqHeader);
  client.print(reqHost);
  client.print(reqType);
  client.print(reqLength);
  client.println();
  client.println(mJson);

  unsigned long timeout = millis();

  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.connected()) {
    // Read all the lines of the reply from server and print them to Serial
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Server Sended");
    } else {
      delay(50);
    }
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

void sendCommandForEsp32(String command, long timeout) {

  while (bluetoothSerial.available()) bluetoothSerial.read(); //clear buffer
  uint8_t idx = 0;
  bool replyMatch = false;
  long timer = millis();
  char replybuffer[255] = {0,};
  bluetoothSerial.print(command);
  Serial.print(F("\t-->")); Serial.println(command);

  while (!replyMatch && (millis() - timer < 1000)) {
    if (bluetoothSerial.available()) {
      replybuffer[idx] = bluetoothSerial.read();
      idx++;
    }
  }
  Serial.print(F("\t <--- ")); Serial.println(replybuffer);
  memset(replybuffer, 0, sizeof(replybuffer));

}

/**
   Test Function
*/
void sendCommandForEsp32Test(String command, long timeout) {

  while (bluetoothSerial.available()) bluetoothSerial.read(); //clear buffer
  uint8_t idx = 0;
  bool replyMatch = false;
  long timer = millis();
  char replybuffer[255] = {0,};
  bluetoothSerial.print(command);
  Serial.print(F("\t-->")); Serial.println(command);

  while (!replyMatch && (millis() - timer < 1000)) {
    if (bluetoothSerial.available()) {
      replybuffer[idx] = bluetoothSerial.read();
      idx++;
    }

  }
  char addressBuffer[30];
  String deviceAddress;
  String deviceBaudrate;
  String temp;
  if (replybuffer[0] == '+') {
    Serial.println("get response");
    if (replybuffer[1] == 'A' && replybuffer[2] == 'D' && replybuffer[3] == 'D' && replybuffer[4] == 'R') {
      for (int i = 6; i < idx; i++) {
        deviceAddress += replybuffer[i];
      }

      deviceAddress.toUpperCase();
      deviceAddress.trim();
      deviceAddress.replace(":", "");

      Serial.println(deviceAddress.substring(0, 2));
      Serial.println(deviceAddress.substring(2, 4));
      Serial.println(deviceAddress.substring(4, 6));
      Serial.println(deviceAddress.substring(6, 8));
      Serial.println(deviceAddress.substring(8, 10));
      Serial.println(deviceAddress.substring(10, 12));
      
      for (int i = 0; i < 6; i++) {
        if (i == 5) {
          temp += deviceAddress.substring(i, i + 2);
        } else {
          temp += deviceAddress.substring(i, i + 2) + ':';
        }
      }

      //      for(int i = 0; i < deviceAddress.length(); i++){
      //        if( i % 2 == 0){
      //          temp[i] = ':';
      //        }
      //        else{
      //          temp[i] = deviceAddress[i];
      //        }
      //      }
      Serial.print("temp --> "); Serial.println(temp);
      deviceAddress = temp;
    }

    else if (replybuffer[1] == 'U' && replybuffer[2] == 'A' && replybuffer[3] == 'R' && replybuffer[4] == 'T') {

      for (int i = 6; i < idx; i++) {
        deviceBaudrate += replybuffer[i];
      }
    }

  }

  Serial.println(deviceAddress);
  Serial.println(deviceBaudrate);
  //  for(int i =0; i<idx; i++){
  //    Serial.println(replybuffer[i]);
  //  }
  Serial.print(F("\t <--- ")); Serial.println(replybuffer);
  memset(replybuffer, 0, sizeof(replybuffer));

}

void setup() {
  byte count = 0;

  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  bluetoothSerial.begin(BLUETOOTH_BAUDRATE, SERIAL_8N1, MCU_RX2, MCU_TX2);
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(button2.PIN, INPUT_PULLUP);
  pinMode(button3.PIN, INPUT_PULLUP);

  //  attachInterruptArg(button1.PIN, isr, &button1, FALLING);
  attachInterrupt(button1.PIN, isr_up, FALLING);
  attachInterrupt(button2.PIN, isr_down, FALLING);
  attachInterrupt(button3.PIN, isr_program, FALLING);

  //  WiFi.begin(ssid, password);
  WiFi.begin(AP_NAME);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wifi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());


}

long system_time = 0;

void loop() {

  if (millis() - system_time > 20000) {
    send_event("bt2_setting_test");
    system_time = millis();
  }




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
    sendCommandForEsp32("AT\r\n", 1000);
    sendCommandForEsp32("AT\r\n", 1000);
    sendCommandForEsp32Test("AT\r\n", 1000);
    sendCommandForEsp32Test("AT+VERSION?\r\n", 1000);
    sendCommandForEsp32Test("AT+NAME?\r\n", 1000);
    sendCommandForEsp32Test("AT+ADDR?\r\n", 1000);
    sendCommandForEsp32Test("AT+UART?\r\n", 1000);
    Serial.printf("Button 3 has been pressed %u times\n", button2.numberKeyPresses);

    button3.pressed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Program Start");
  }

}
