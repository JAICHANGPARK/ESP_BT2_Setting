
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

#define DEBUG

#define AP_NAME       "Himart-WiFi"
#define IFTTT_HOST    "maker.ifttt.com"
#define IFTTT_KEY     ""

#define MCU_RX2       16
#define MCU_TX2       17

#define BLUETOOTH_BAUDRATE          38400

const char* ssid     = "";
const char* password = "";
const char* host = "maker.ifttt.com";


long intterrupt_time = 0;
long t = 0;

volatile uint32_t deviceId = 10000;
int iDeviceBaudrate = 115200;


String deviceName;
String deviceAddress;
String deviceBaudrate;
String deviceVersion;


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
#ifdef DEBUG
  Serial.print("Connecting to");
  Serial.println(host);
#endif
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
#ifdef DEBUG
    Serial.println("connection failed");
#endif
    return;
  }

  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += IFTTT_KEY;
  String mJson = String("{\"value1\" : \"") + "Test\", " +
                 String("\"value2\" : \"") + "Test\", " +
                 String("\"value3\" : \"") + "Test\"} ";
#ifdef DEBUG
  Serial.print("Requesting URL: ");
  Serial.println(url);
#endif
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
#ifdef DEBUG
      Serial.println(">>> Client Timeout !");
#endif
      client.stop();
      return;
    }
  }

  while (client.connected()) {
    // Read all the lines of the reply from server and print them to Serial
    if (client.available()) {
#ifdef DEBUG
      String line = client.readStringUntil('\r');
      Serial.print(line);
#endif
    } else {
      delay(50);
    }
  }
#ifdef DEBUG
  Serial.println();
  Serial.println("closing connection");
#endif
  client.stop();
}

void send_event(const char *event, String type, String address, String baud, String v) {
#ifdef DEBUG
  Serial.print("Connecting to");
  Serial.println(host);
#endif
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
#ifdef DEBUG
    Serial.println("connection failed");
#endif
    return;
  }

  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += IFTTT_KEY;

  String mJson = String("{\"value1\" : \"") + "AS" + String(deviceId) + " (" + type + "_" + v + ")" + "\", " +
                 String("\"value2\" : \"") + address + "\", " +
                 String("\"value3\" : \"") + baud + "\"} ";
#ifdef DEBUG
  Serial.print(mJson);
  Serial.print("Requesting URL: ");
  Serial.println(url);
#endif

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
#ifdef DEBUG
      Serial.println(">>> Client Timeout !");
#endif
      client.stop();
      return;
    }
  }

  while (client.connected()) {
    // Read all the lines of the reply from server and print them to Serial
    if (client.available()) {
      String line = client.readStringUntil('\r');
#ifdef DEBUG
      Serial.print(line);
#endif
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Server Sended");
      return;
    } else {
      delay(50);
    }
  }
#ifdef DEBUG
  Serial.println();
  Serial.println("closing connection");
#endif
  client.stop();
}

void sendCommandForEsp32(String command, long timeout) {

  while (bluetoothSerial.available()) bluetoothSerial.read(); //clear buffer
  uint8_t idx = 0;
  bool replyMatch = false;
  long timer = millis();
  char replybuffer[255] = {0,};
  bluetoothSerial.print(command);
#ifdef DEBUG
  Serial.print(F("\t-->")); Serial.println(command);
#endif

  while (!replyMatch && (millis() - timer < 1000)) {
    if (bluetoothSerial.available()) {
      replybuffer[idx] = bluetoothSerial.read();
      idx++;
    }
  }
#ifdef DEBUG
  Serial.print(F("\t <--- ")); Serial.println(replybuffer);
#endif
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
#ifdef DEBUG
  Serial.print(F("\t-->")); Serial.println(command);
#endif
  while (!replyMatch && (millis() - timer < 1000)) {
    if (bluetoothSerial.available()) {
      replybuffer[idx] = bluetoothSerial.read();
      idx++;
    }

  }
  char addressBuffer[30];

  String temp;

  if (replybuffer[0] == '+') {
#ifdef DEBUG
    Serial.println("get response");
#endif
    if (replybuffer[1] == 'A' && replybuffer[2] == 'D' && replybuffer[3] == 'D' && replybuffer[4] == 'R') {
      for (int i = 6; i < idx; i++) {
        deviceAddress += replybuffer[i];
      }

      int index = deviceAddress.indexOf('\r');
      deviceAddress = deviceAddress.substring(0, index);

      deviceAddress.toUpperCase();
      deviceAddress.trim();
      deviceAddress.replace(":", "");
      index = 0;
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];
      temp += ':';
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];
      temp += ':';
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];
      temp += ':';
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];
      temp += ':';
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];
      temp += ':';
      temp += deviceAddress[index++];
      temp += deviceAddress[index++];

      Serial.print("temp --> : ");
      Serial.println(temp);




      //
      //      for (int i = 0; i < 6; i++) {
      //        if (i == 5) {
      //          temp += deviceAddress.substring(i, i + 2);
      //        } else {
      //          temp += deviceAddress.substring(i, i + 2) + ':';
      //        }
      //      }


      //      Serial.println(deviceAddress.substring(0, 2));
      //      Serial.println(deviceAddress.substring(2, 4));
      //      Serial.println(deviceAddress.substring(4, 6));
      //      Serial.println(deviceAddress.substring(6, 8));
      //      Serial.println(deviceAddress.substring(8, 10));
      //      Serial.println(deviceAddress.substring(10, 12));

      //      for(int i = 0; i < deviceAddress.length(); i++){
      //        if( i % 2 == 0){
      //          temp[i] = ':';
      //        }
      //        else{
      //          temp[i] = deviceAddress[i];
      //        }
      //      }
#ifdef DEBUG
      Serial.print("temp --> "); Serial.println(temp);
#endif
      deviceAddress = temp;
    }

    else if (replybuffer[1] == 'U' && replybuffer[2] == 'A' && replybuffer[3] == 'R' && replybuffer[4] == 'T') {

      for (int i = 6; i < idx; i++) {
        deviceBaudrate += replybuffer[i];
      }
      int index = deviceBaudrate.indexOf('\r');
      deviceBaudrate = deviceBaudrate.substring(0, index);

#ifdef DEBUG
      Serial.println(index);
      Serial.print("deviceBaud --> "); Serial.print(deviceBaudrate);
#endif

    }

    else if (replybuffer[1] == 'V' && replybuffer[2] == 'E' && replybuffer[3] == 'R' && replybuffer[4] == 'S') {

      for (int i = 9; i < idx; i++) {
        deviceVersion += replybuffer[i];
      }
      int index = deviceVersion.indexOf('\r');
      deviceVersion = deviceVersion.substring(0, index);

#ifdef DEBUG
      Serial.println(index);
      Serial.print("deviceVersion --> "); Serial.print(deviceVersion);
#endif
    }

    else if (replybuffer[1] == 'N' && replybuffer[2] == 'A' && replybuffer[3] == 'M' && replybuffer[4] == 'E') {
      for (int i = 9; i < idx; i++) {
        deviceName += replybuffer[i];
      }
      int index = deviceName.indexOf('\r');
      Serial.println(index);
      Serial.print("deviceName --> "); Serial.print(deviceName);
      deviceName = deviceName.substring(0, index);
    }


  }
#ifdef DEBUG
  Serial.println(deviceAddress);
  Serial.println(deviceBaudrate);
  Serial.println(deviceVersion);
  Serial.println(deviceName);
  //  for(int i =0; i<idx; i++){
  //    Serial.println(replybuffer[i]);
  //  }
  Serial.print(F("\t <--- ")); Serial.println(replybuffer);
#endif
  memset(replybuffer, 0, sizeof(replybuffer));

}



void setup() {
  byte count = 0;

  lcd.begin();
  lcd.backlight();

#ifdef DEBUG
  Serial.begin(115200);
#endif
  bluetoothSerial.begin(BLUETOOTH_BAUDRATE, SERIAL_8N1, MCU_RX2, MCU_TX2);
  pinMode(button1.PIN, INPUT_PULLUP);
  pinMode(button2.PIN, INPUT_PULLUP);
  pinMode(button3.PIN, INPUT_PULLUP);

  //  attachInterruptArg(button1.PIN, isr, &button1, FALLING);
  attachInterrupt(button1.PIN, isr_up, FALLING);
  attachInterrupt(button2.PIN, isr_down, FALLING);
  attachInterrupt(button3.PIN, isr_program, FALLING);

  //  WiFi.begin(ssid, password);

  int lcdCount = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wifi Connecting");
  WiFi.begin(AP_NAME);

  while (WiFi.status() != WL_CONNECTED) {
    if (lcdCount == 17) {
      lcdCount = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wifi Connecting");
    }
    lcd.setCursor(lcdCount, 1);
    lcd.print(".");
    lcdCount++;
#ifdef DEBUG
    Serial.print(".");
#endif
    delay(500);
  }
#ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wifi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());


}

long system_time = 0;

void loop() {

  //  if (millis() - system_time > 20000) {
  //    send_event("bt2_setting_test");
  //    system_time = millis();
  //  }

  if (button1.pressed) {
#ifdef DEBUG
    Serial.printf("Button 1 has been pressed %u times\n", button1.numberKeyPresses);
#endif
    button1.pressed = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DeviceID : ");
    lcd.setCursor(0, 1);
    lcd.print("AS : " + String(deviceId));
    //    lcd.print("Button 1 ->" + String(button1.numberKeyPresses));
  }

  if (button2.pressed) {
    if (deviceId <= 9999) {
      deviceId = 10000;
    }
#ifdef DEBUG
    Serial.printf("Button 2 has been pressed %u times\n", button2.numberKeyPresses);
#endif
    button2.pressed = false;


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DeviceID : ");
    lcd.setCursor(0, 1);
    lcd.print("AS : " + String(deviceId));
  }

  if (button3.pressed) {

    String setNameCommand = "AT+NAME=" + String("AS") + String(deviceId) + "\r\n";
    String setBaudrateCommand = "AT+UART=" + String(iDeviceBaudrate) + ",0,0" + "\r\n";

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Program Start");
    lcd.setCursor(0, 1);
    lcd.print("AT Setting");

    sendCommandForEsp32("AT\r\n", 1000);
    sendCommandForEsp32("AT\r\n", 1000);
    sendCommandForEsp32Test("AT\r\n", 1000);

    sendCommandForEsp32Test(setNameCommand, 1000);
    sendCommandForEsp32Test(setBaudrateCommand, 1000);

    sendCommandForEsp32Test("AT+VERSION?\r\n", 1000);
    sendCommandForEsp32Test("AT+NAME?\r\n", 1000);
    sendCommandForEsp32Test("AT+ADDR?\r\n", 1000);
    sendCommandForEsp32Test("AT+UART?\r\n", 1000);

#ifdef DEBUG
    Serial.printf("Button 3 has been pressed %u times\n", button2.numberKeyPresses);
#endif

    button3.pressed = false;

    /**
       void send_event(const char *event, String type, String address, String baud, String v)
    */
    if (deviceAddress != "" && deviceBaudrate != "" && deviceVersion != "") {
      send_event("bt2_setting_test", "HC-05", deviceAddress, deviceBaudrate, deviceVersion);
      deviceAddress = "";
      deviceBaudrate = "";
      deviceVersion = "";
      deviceName = "";
    }

  }

}
