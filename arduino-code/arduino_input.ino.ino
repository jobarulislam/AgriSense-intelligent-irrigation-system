#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <DHT.h>


#define SOILMOISTUREPIN A2
#define WATERLEVELPIN A5
#define MOTORPIN 13

#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_MIN 600
#define SOIL_MAX 1000

SoftwareSerial ESPSerial(3, 4);

int chk;
float hum;
float temp;
float soilMoisture;
float waterLevel;
bool motor = false;
void setup() {
  Serial.begin(9600);
  ESPSerial.begin(57600);
  pinMode(MOTORPIN, OUTPUT);
  digitalWrite(MOTORPIN, LOW);
  dht.begin();
}

unsigned long previousMillis = 0; // Stores the last time the LED was toggled
const long interval = 3000; 

void loop() {
  unsigned long currentMillis = millis();
  // Serial.println(currentMillis - previousMillis);
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis; 
    sendDataToServer();
  }

  if(ESPSerial.available()){
    // Serial.println("Data received");
    String data = ESPSerial.readStringUntil('\r\n');
    data.trim();
    if(data.equals("M1")){
      motor = true;
      Serial.println("Motor Turned on");
    } else if(data == "M0"){
      motor = false;
      Serial.println("Motor Turned off");
    } else{
      Serial.print("NodeMcu -> not for motor: ");
      
    }
    Serial.println(data);
    // Serial.println(data.length());
    // printAscii(data);
  }
  digitalWrite(MOTORPIN, motor);

}

void sendDataToServer(){
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  float soil = analogRead(SOILMOISTUREPIN);
  soil = max(soil, SOIL_MIN);
  soil = min(soil, SOIL_MAX);
  soilMoisture = map(soil, SOIL_MAX, SOIL_MIN, 0, 100);
  waterLevel = analogRead(WATERLEVELPIN);

  StaticJsonDocument<200> jsonDoc;
  // Add sensor values to the JSON object
  jsonDoc["temperature"] = temp;
  jsonDoc["humidity"] = hum;
  jsonDoc["soilMoisture"] = soilMoisture;
  jsonDoc["waterLevel"] = waterLevel;
  jsonDoc["motorState"] = motor;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  ESPSerial.println(jsonString);
  printSensorsData();
}

void printSensorsData(){
  Serial.print("Humidity: ");
  Serial.println(hum);

  Serial.print("Temperature: ");
  Serial.println(temp);

  Serial.print("Water level: ");
  Serial.println(waterLevel);

  Serial.print("Mositure : ");
  Serial.println(soilMoisture);

  Serial.print("Motor State : ");
  Serial.println(motor);
}

void printAscii(String str){
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];           // Get each character
    int asciiValue = (int)c;   // Cast character to int to get ASCII value
    Serial.print("Character: ");
    Serial.print(c);
    Serial.print(" -> ASCII: ");
    Serial.println(asciiValue);
  }
}
