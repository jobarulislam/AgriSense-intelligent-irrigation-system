#include <SoftwareSerial.h>

SoftwareSerial espSerial(D2, D1); // RX, TX for ESP8266

void setup() {
  Serial.begin(19200);      // Communication with Serial Monitor
  espSerial.begin(4800);   // Communication with Arduino
  Serial.println("ESP8266 ready. Type something to send to Arduino.");
}

void loop() {
  // Forward data from Serial Monitor to Arduino
  if (Serial.available()) {
    String data = Serial.readString();
    espSerial.print(data); // sending data
    Serial.println("Sent to Arduino: " + data);
  }

  // Forward data from Arduino to Serial Monitor
  if (espSerial.available()) {
    String data = espSerial.readString();
    Serial.println("Received from Arduino: " + data);
  }
}
