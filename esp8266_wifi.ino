#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Wi-Fi credentials
const char* ssid = "Dr. Askar ali";
const char* password = "askarikamal.277";

// Server URL
const char* serverURL = "https://jsonplaceholder.typicode.com/todos/1";

// Relay pin
#define RELAY_PIN D5

SoftwareSerial espSerial(D2, D1); // RX, TX for ESP8266

// Create WiFiClient instance
WiFiClient wifiClient;

void setup() {
  Serial.begin(19200);     // Debugging via Serial Monitor
  espSerial.begin(4800);  // Communication with Arduino
  Serial.println("ESP8266 ready. Type something to send to Arduino.");
  
  // wifi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
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

    // fetch data
    if (WiFi.status() == WL_CONNECTED) {
      getData();
    } else {
      Serial.println("Wi-Fi not connected!");
    }
  }
}

void getData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    // Initialize GET request
    http.begin(wifiClient, serverURL);
    http.addHeader("Content-Type", "application/json");

    // Send the request and get the response
    int httpCode = http.GET();

    // Check the response code
    if (httpCode > 0) {
      Serial.print("HTTP Code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK) { // HTTP 200
        String payload = http.getString(); // Get the response payload
        Serial.println("Response:");
        Serial.println(payload);
      }
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(http.errorToString(httpCode).c_str());
    }

    http.end(); // End the connection
  }

  delay(5000); // Wait 5 seconds before the next request
}

