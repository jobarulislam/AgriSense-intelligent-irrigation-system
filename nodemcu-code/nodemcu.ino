#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <SoftwareSerial.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
SoftwareSerial ESPSerial(D2, D1);

#define USE_SERIAL Serial
#define ssid "HONOR X6b"
#define pass "whydouneed"
#define ws_host "20.2.89.216"
#define ws_port 8000

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("Connected");
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to arduino
      ESPSerial.printf("%s", payload);
      // if (ESPSerial.available()) {
  		// 	USE_SERIAL.println("DATA TRANSFERRING...");
      //   ESPSerial.printf("",payload);
      // }
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
    case WStype_PING:
        // pong will be send automatically
        USE_SERIAL.printf("[WSc] get ping\n");
        break;
    case WStype_PONG:
        // answer to a ping we send
        USE_SERIAL.printf("[WSc] get pong\n");
        break;
  }

}

void setup() {
  
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);
	ESPSerial.begin(57600); // UART Serial Comm.



	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP(ssid, pass);

  USE_SERIAL.print("HELLO");

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
  Serial.println(ws_host);
  Serial.println(ws_port);
	webSocket.begin(ws_host, ws_port, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);

}

void loop() {
	webSocket.loop();
  // USE_SERIAL.print(".");

  if (ESPSerial.available()) {
    String data = ESPSerial.readStringUntil('\r\n');
    USE_SERIAL.println(data);
  
    if (webSocket.isConnected() && isValid(data)) {
      webSocket.sendTXT(data);
    }
  }

  // if (USE_SERIAL.available()) {
  //   if (webSocket.isConnected()) {
  //     String data = USE_SERIAL.readStringUntil('\r\n');
  //     webSocket.sendTXT(data);
  //   }
  // }
}

bool isValid(String data) {
  // Create a StaticJsonDocument for parsing
    StaticJsonDocument<200> doc; // Adjust size based on JSON complexity

    // Parse the JSON string
    DeserializationError error = deserializeJson(doc, data);

    // Check if parsing was successful
    if (error) {
        // Serial.print("JSON parsing failed: ");
        // Serial.println(error.c_str());
        return false;
    } else {
        // Serial.println("JSON parsing successful!");
        return true;
    }
}
