#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "uRTCLib.h"

uRTCLib rtc(0x68);

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

const int MAX_DUTY_CYCLE = (int)(pow(2, resolution) - 1);

int dutyCycle = 100;

#define enB 33
#define in3 35
#define in4 32

int rotDirection = 0;
int pressed = false;

String TimeString;

DynamicJsonDocument doc(2048);  

JsonArray dataArray = doc.to<JsonArray>();

const char* ssid = "Gx";
const char* password = "mostafa71";

WebServer server(80);

void generateJSON(DynamicJsonDocument& doc) {
  JsonObject data = doc.createNestedObject();
  data["platform"] = "EX01";
  data["time"] = MeasureTime();

  JsonObject pressure = data.createNestedObject("pressure");
  pressure["value"] = ReadPressure();
  pressure["unit"] = "kpa";

  JsonObject depth = data.createNestedObject("depth");
  depth["value"] = CalculateDepth(pressure["value"]);
  depth["unit"] = "Meters";
}

void getAllData() {
  try {
    String jsonData;
    serializeJson(doc, jsonData);  
    server.send(200, "application/json", jsonData);
  } 
  catch (const std::exception& e) {
    Serial.println("An exception occurred in the getAllData function:");
    Serial.println(e.what());
    StaticJsonDocument<200> errorJson;
    errorJson["error"] = "Error";
    String error;
    serializeJson(errorJson, error);
    server.send(500, "application/json", error);
  }
}


void start() {
  try {
    for(int counter = 0; counter <= 5; counter++) {
      GoForward();
      generateJSON(doc);
      delay(1000);
    }

    Stopp();
    
    StaticJsonDocument<200> msg;
    msg["msg"] = "The mission completed successfully";
    String jsonString;
    serializeJson(msg, jsonString);
    server.send(200, "application/json", jsonString);
  } 
  catch (const std::exception& e) {
    StaticJsonDocument<200> errorJson;
    errorJson["error"] = "The mission is not start";
    String error;
    serializeJson(errorJson, error);
    server.send(500, "application/json", error);
  }
}

void heartbeat() {
   
    StaticJsonDocument<200> msg;
    msg["msg"] = "The connection succeeded";
    String jsonString;
    serializeJson(msg, jsonString);
    server.send(200, "application/json", jsonString);
}



void setup() {
  Serial.begin(115200);
  delay(10);
  
  Wire.begin();
  
  SetupMotor();

  SetupRTC();
// server setup
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Routes
  server.on("/getdata", HTTP_GET, getAllData);
  server.on("/start", HTTP_GET, start);
  server.on("/ping", HTTP_GET, heartbeat);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}


void GoForward(){
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  for(int dutyCycle = 0; dutyCycle <= MAX_DUTY_CYCLE; dutyCycle += 2){   
    ledcWrite(ledChannel, dutyCycle);
    delay(2);
  }  
}

void GoBackwards(){
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  for(int dutyCycle = 0; dutyCycle <= MAX_DUTY_CYCLE; dutyCycle += 2){   
    ledcWrite(ledChannel, dutyCycle);
    delay(2);
  }  
}

void Stopp(){
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW); 
   ledcWrite(ledChannel, 0); 
}

float ReadPressure() {

  // THIS CODE DOES NOT WORK
  
  Wire.beginTransmission(0x76); 
  Wire.write(0x48);
  Wire.endTransmission();
  
  delay(40);
  
  Wire.beginTransmission(0x76);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.requestFrom(0x76, 3);
  
  if (Wire.available() == 3) {
    byte msb = Wire.read();
    byte lsb = Wire.read();
    byte crc = Wire.read();
    
    unsigned int rawPressure = ((msb << 8) | lsb) & 0x3FFF;
    float pressure = ((rawPressure - 1638.0) / 13107.0) + 10.0;
    
    return pressure;
  }
  return 0.0; 
}

float CalculateDepth(float pressure) {

  // THIS CODE DOES NOT WORK
  
  float seaLevelPressure = 1013.25; 
  float Depth = (1.0 - pow(pressure / seaLevelPressure, 0.1903)) * 44330.0;
  return Depth;
}

// note

String MeasureTime(){
  rtc.refresh();

  TimeString = String(rtc.hour() - 2) + ":"
             + String(rtc.minute()) + ":"
             + String(rtc.second()) + " UTC";
 delay(50);
  return TimeString;

  
}

void SetupRTC(){
  #ifdef ARDUINO_ARCH_ESP8266
    URTCLIB_WIRE.begin(0, 2); // D3 and D4 on ESP8266
  #else
    URTCLIB_WIRE.begin();
  #endif

  //  LEAVE THIS UNCOMMENTD IF YOU WANT TO SET TIME
  //  rtc.set(0, 57, 5, 0, 13, 4, 24);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}

void SetupMotor(){
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enB, ledChannel);
}
