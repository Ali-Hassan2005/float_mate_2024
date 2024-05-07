#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "uRTCLib.h"
#include "math.h"


// ***** CONSTANTS AND VARIABLES FOR MOTORS AND SENSORS ******
const int MovingTime = 15000; // Time in milliseconds for forward direction (15 seconds * 1000 milliseconds/second)
const int stopTime = 30000;    // Time in milliseconds to stop (1 minute * 1000 milliseconds/minute)

#define in3 32
#define in4 33

// ** ADRESSES **
uRTCLib rtc(0x68);

/////////// CHANGE THOSE FOR CALIBRATION BEFORE MISSION ////////////////////////

float DepthZero = 99.60;
float PressureZero = 100.60;

#define GravityConstant 9.81
#define WaterDensity 1000
#define CalibrationConstant 10

// -----------------------------------------------------------------------------

// Leave this here please, It's needed as a global variable for the read function
float TEMPREAL;

///////////////////// TIME STRING ///////////////
String TimeString;

// ***** SERVER OBJECTS ******

DynamicJsonDocument doc(2048);  
JsonArray dataArray = doc.to<JsonArray>();
WebServer server(80);

/*
 * WIFI CREDENTIALS SETUP
 * 
 */
 
const char* ssid = "Etisalat-Uq5q";
const char* password = "7k9y5ENP";

// ---------------------------------------------

/*
 * JSON GENERATION FUNCTIONS
 * 
 */

void generateJSON(DynamicJsonDocument& doc) {
  JsonObject data = doc.createNestedObject();
  data["platform"] = "EX01";
  data["time"] = MeasureTime();

  JsonObject pressure = data.createNestedObject("pressure");
  pressure["value"] = (MeasurePressure() - 901);
  pressure["unit"] = "kpa";

  JsonObject depth = data.createNestedObject("depth");
  depth["value"] = (GetDepth(pressure["value"]));
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

// ---------------------------------------------

/*
 * CALLBACK FUNCTIONS FOR BUTTONS
 * 
 * 
 */

void start() {

  // CW is GoUp
  // CCW is GoDown
  
  // GoUp --> ClockWise --> Goes up
  // GoDown --> CounterClockWise --> Goes Down
  
  // We should get a certain Depth on sensor while motor is CW and add on it 20Kpa (2 meters) then we reverse the motor direction to CW
   
  try {
    // Move forward for 15 seconds
  PushUp();
  Serial.println("Time to Push Up");
  delay(MovingTime);

  // Stop the motor for 30 seconds
  Stopp();
  Serial.println("Time to Stop Pushing");
  delay(stopTime);

  // Move backward for 15 seconds
  PushDown();
  Serial.println("Time to Push Down");
  delay(MovingTime);
    
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
void deleteDate() {
   doc.clear();
   StaticJsonDocument<200> msg;
    msg["msg"] = "Deleted successfully";
    String jsonString;
    serializeJson(msg, jsonString);
    server.send(200, "application/json", jsonString);
}

void sendData() {
  String requestBody = server.arg("plain");
  
  DynamicJsonDocument jsonBuffer(2048);
  DeserializationError error = deserializeJson(jsonBuffer, requestBody);

   
  if (error) {
    StaticJsonDocument<200> errorJson;
    errorJson["error"] = "Failed to parse JSON";
    String errorResponse;
    serializeJson(errorJson, errorResponse);
    server.send(400, "application/json", errorResponse);
    return;
  }
  

  // First key is PressureZero, second key is DepthZero 
  PressureZero = jsonBuffer["key1"];
  DepthZero = jsonBuffer["key2"];
  
  StaticJsonDocument<200> responseJson;
  responseJson["msg"] = "success";
   
  String response;
  serializeJson(responseJson, response);

  
  server.send(200, "application/json", response);
}

void funUP() {
   
     
    //
}


void fundown() {
   
   //
}


// --------------------------------------------------------------------------------------------

   
void setup() {
  
// ********* SERIAL, SENSORS AND MOTORS SETUP **********
  Serial.begin(115200);
  delay(10);
  Wire.begin();
  SetupMotor();
  SetupRTC();
  SPI.begin(); //see SPI library details on arduino.cc for details
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV32); //divide 16 MHz to communicate on 500 kHz
  
  delay(100);
  
// ********** SERVER SETUP **************
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
  server.on("/sendVal", HTTP_POST, sendData);
  server.on("/deleteDate",HTTP_GET, deleteDate);
  server.on("/up",HTTP_GET, funUP);
  server.on("/down",HTTP_GET, fundown );


  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(WiFi.status() != WL_CONNECTED){
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
    server.begin();
    Serial.println("HTTP server started");
  
  }

  Serial.print("Pressure: ");
    Serial.print(MeasurePressure());
    Serial.println("Kpa");

    Serial.print(", Depth: ");
    Serial.print(GetDepth(MeasurePressure()));
    Serial.println("Meters");
    
}


/*
 * MOTOR FUNCTIONS
 * 
 * 
 */
 
void GoUp(){
  //////////////////// PUSH DOWN ////////////////

  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  for(int dutyCycle = 0; dutyCycle <= MAX_DUTY_CYCLE; dutyCycle += 2){   
    ledcWrite(ledChannel, dutyCycle);
    delay(2);
  }  
}

void GoDown(){
  //////////////////// PULL UP ////////////////

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

void SetupMotor(){
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enB, ledChannel);
}

// ---------------------------------------------------------------------------------------------

/*
 * PRESSURE SENSOR AND DEPTH CALCULATIONS
 * 
 * 
 */

int MeasurePressure(){
  resetsensor(); //resets the sensor - caution: afterwards mode = SPI_MODE0!

  //Calibration word 1
  unsigned int result1 = 0;
  unsigned int inbyte1 = 0;
  SPI.transfer(0x1D); //send first byte of command to get calibration word 1
  SPI.transfer(0x50); //send second byte of command to get calibration word 1
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  result1 = SPI.transfer(0x00); //send dummy byte to read first byte of word
  result1 = result1 << 8; //shift returned byte 
  inbyte1 = SPI.transfer(0x00); //send dummy byte to read second byte of word
  result1 = result1 | inbyte1; //combine first and second byte of word

  resetsensor(); //resets the sensor

  //Calibration word 2; see comments on calibration word 1
  unsigned int result2 = 0;
  byte inbyte2 = 0; 
  SPI.transfer(0x1D);
  SPI.transfer(0x60);
  SPI.setDataMode(SPI_MODE1); 
  result2 = SPI.transfer(0x00);
  result2 = result2 <<8;
  inbyte2 = SPI.transfer(0x00);
  result2 = result2 | inbyte2;

  resetsensor(); //resets the sensor

  //Calibration word 3; see comments on calibration word 1
  unsigned int result3 = 0;
  byte inbyte3 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0x90); 
  SPI.setDataMode(SPI_MODE1); 
  result3 = SPI.transfer(0x00);
  result3 = result3 <<8;
  inbyte3 = SPI.transfer(0x00);
  result3 = result3 | inbyte3;

  resetsensor(); //resets the sensor

  //Calibration word 4; see comments on calibration word 1
  unsigned int result4 = 0;
  byte inbyte4 = 0;
  SPI.transfer(0x1D);
  SPI.transfer(0xA0);
  SPI.setDataMode(SPI_MODE1); 
  result4 = SPI.transfer(0x00);
  result4 = result4 <<8;
  inbyte4 = SPI.transfer(0x00);
  result4 = result4 | inbyte4;
  
  //now we do some bitshifting to extract the calibration factors 
  //out of the calibration words;
  long c1 = (result1 >> 1) & 0x7FFF;
  long c2 = ((result3 & 0x003F) << 6) | (result4 & 0x003F);
  long c3 = (result4 >> 6) & 0x03FF;
  long c4 = (result3 >> 6) & 0x03FF;
  long c5 = ((result1 & 0x0001) << 10) | ((result2 >> 6) & 0x03FF);
  long c6 = result2 & 0x003F;
  
  resetsensor(); //resets the sensor

  //Pressure:
  unsigned int presMSB = 0; //first byte of value
  unsigned int presLSB = 0; //last byte of value
  unsigned int D1 = 0;
  SPI.transfer(0x0F); //send first byte of command to get pressure value
  SPI.transfer(0x40); //send second byte of command to get pressure value
  delay(35); //wait for conversion end
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  presMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
  presMSB = presMSB << 8; //shift first byte
  presLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
  D1 = presMSB | presLSB; //combine first and second byte of value
  //Serial.print("D1 - Pressure raw = ");
  //Serial.println(D1);

  resetsensor(); //resets the sensor  

  //Temperature:
  unsigned int tempMSB = 0; //first byte of value
  unsigned int tempLSB = 0; //last byte of value
  unsigned int D2 = 0;
  SPI.transfer(0x0F); //send first byte of command to get temperature value
  SPI.transfer(0x20); //send second byte of command to get temperature value
  delay(35); //wait for conversion end
  SPI.setDataMode(SPI_MODE1); //change mode in order to listen
  tempMSB = SPI.transfer(0x00); //send dummy byte to read first byte of value
  tempMSB = tempMSB << 8; //shift first byte
  tempLSB = SPI.transfer(0x00); //send dummy byte to read second byte of value
  D2 = tempMSB | tempLSB; //combine first and second byte of value
  //Serial.print("D2 - Temperature raw = ");
  //Serial.println(D2); //voila!

  //calculation of the real values by means of the calibration factors and the maths
  //in the datasheet. const MUST be long
  const long UT1 = (c5 << 3) + 20224;
  const long dT = D2 - UT1;
  const long TEMP = 200 + ((dT * (c6 + 50)) >> 10);
  const long OFF  = (c2 * 4) + (((c4 - 512) * dT) >> 12);
  const long SENS = c1 + ((c3 * dT) >> 10) + 24576;
  const long X = (SENS * (D1 - 7168) >> 14) - OFF;
  long PCOMP = ((X * 10) >> 5) + 2500;
  TEMPREAL = TEMP/10;
  float PCOMPHG = PCOMP * 750.06 / 10000; // mbar*10 -> mmHg === ((mbar/10)/1000)*750/06

  PCOMP = (PCOMP / 10) + 3;
  
  return PCOMP; // RETURNS PRESSURE IN mbar // Multiply by 0.1 to get Kpa
}

float GetDepth(float press) {
  return roundf(((press - PressureZero / WaterDensity*GravityConstant) - DepthZero) / CalibrationConstant);
}

void resetsensor() //this function keeps the sketch a little shorter
{
  SPI.setDataMode(SPI_MODE0); 
  SPI.transfer(0x15);
  SPI.transfer(0x55);
  SPI.transfer(0x40);
}

//------------------------------------------------------------------------------------------------------

/*
 * TIME MEASUREMENT AND SETUP FOR RTC
 * 
 */

String MeasureTime(){
  rtc.refresh();

  TimeString = String(rtc.hour() - 2) + ":"
             + String(rtc.minute()) + ":"
             + String(rtc.second()) + " UTC";
 delay(50);
  return TimeString;

  
}

void SetupRTC(){
  rtc.set_rtc_address(0x68);

	// set RTC Model
	rtc.set_model(URTCLIB_MODEL_DS1307);

	// refresh data from RTC HW in RTC class object so flags like rtc.lostPower(), rtc.getEOSCFlag(), etc, can get populated
	rtc.refresh();

  #ifdef ARDUINO_ARCH_ESP8266
    URTCLIB_WIRE.begin(0, 2); // D3 and D4 on ESP8266
  #else
    URTCLIB_WIRE.begin();
  #endif

  if (rtc.enableBattery()) {
		Serial.println("Battery activated correctly.");
	} else {
		Serial.println("ERROR activating battery.");
	}

	// Check whether OSC is set to use VBAT or not
	if (rtc.getEOSCFlag())
		Serial.println(F("Oscillator will not use VBAT when VCC cuts off. Time will not increment without VCC!"));
	else
		Serial.println(F("Oscillator will use VBAT when VCC cuts off."));

	Serial.print("Lost power status: ");
	if (rtc.lostPower()) {
		Serial.print("POWER FAILED. Clearing flag...");
		rtc.lostPowerClear();
		Serial.println(" done.");
	} else {
		Serial.println("POWER OK");
	}

  //  LEAVE THIS UNCOMMENTD IF YOU WANT TO SET TIME
    rtc.set(0, 52, 7, 0, 13, 4, 24);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}


//-----------------------------------------------------------------------------------------------------------
