#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 3); // RX, TX for Arduino

void setup() {
  Serial.begin(9600);      // Communication with Serial Monitor
  espSerial.begin(4800);   // Communication with ESP8266
  Serial.println("Arduino ready. Type something to send to ESP8266.");
}

void loop() {
  // Forward data from Serial Monitor to ESP8266
  if (Serial.available()) {
    String data = Serial.readString();
    espSerial.print(data); // sending data
    Serial.println("Sent to ESP8266: " + data);
  }

  // Forward data from ESP8266 to Serial Monitor
  if (espSerial.available()) {
    String data = espSerial.readString();
    Serial.println("Received from ESP8266: " + data);
  }
}
