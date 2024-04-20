#include <SoftwareSerial.h>
#include <SPI.h>
// software serial #1: RX = digital pin 7, TX = digital pin 8
SoftwareSerial PortOne(7, 8);

// software serial #2: RX = digital pin 5, TX = digital pin 6
// on the Mega, use other pins instead, since 8 and 9 don't work on the Mega
SoftwareSerial PortTwo(5, 6);

// generate a MCLK signal pin
const int clock = 9;
 
// Leave this here please, It's needed as a global variable for the read function
float TEMPREAL;


/////////// CHANGE THOSE FOR CALIBRATION BEFORE MISSION ////////////////////////

float DepthZero = 99.60;
float PressureZero = 100.60;

#define GravityConstant 9.81
#define WaterDensity 1000
#define CalibrationConstant 10

// -----------------------------------------------------------------------------


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  PortOne.begin(115200);
  PortTwo.begin(115200);
}

void loop() {
  TCCR1B = (TCCR1B & 0xF8) | 1 ; //generates the MCKL signal
  analogWrite (clock, 128) ; 
  
  PortTwo.listen();
  Serial.println();
  while (PortTwo.available() > 0){
    char Command = PortTwo.read();
    if(Command == 'M'){
      int Pressure = MeasurePressure();
      float PressureKpa = (float)Pressure * 0.1;
      float Depth = GetDepth(PressureKpa);
      SendAltitudeAndDepth(PressureKpa, Depth);
    }
    
  else if(Command == 'C') {
      Serial.println("Calibration Process begins..");
      Serial.println("Send the Value for PZero and DZero on Port1 (e.g., P100.60, D99.60)");

      // Variables to store extracted numbers
      float extractedValue;
      char receivedString[10];  // Buffer to hold received string
      int index = 0;

      // Clear buffer before receiving
      memset(receivedString, 0, sizeof(receivedString));

      // Read calibration data until newline or buffer full
      while (PortOne.available() && index < sizeof(receivedString) - 1) {
        char inByte = PortOne.read();
        if (inByte == '\n') {
          break;
        }
        receivedString[index++] = inByte;
      }

      // Check if a string was received
      if (index > 0) {
        // Extract the number after the letter (assuming format "PXXX.XX" or "DXXX.XX")
        if (receivedString[0] == 'P') {
          extractedValue = atof(&receivedString[1]);
          PressureZero = extractedValue;
          Serial.print("PZero set to: ");
          Serial.println(PressureZero);
        } else if (receivedString[0] == 'D') {
          extractedValue = atof(&receivedString[1]);
          DepthZero = extractedValue;
          Serial.print("DZero set to: ");
          Serial.println(DepthZero);
        } else {
          Serial.println("Invalid format. Please use 'P' or 'D' followed by a number.");
        }
    }
  }
 }
}

void SendAltitudeAndDepth(float PressureKpa, float Depth){
  PortOne.print("P");
  PortOne.println(PressureKpa);
  PortOne.print("D");
  PortOne.println(Depth);
}

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
  return ((press - PressureZero / WaterDensity*GravityConstant) - DepthZero) / CalibrationConstant;
 }

void resetsensor() //this function keeps the sketch a little shorter
{
  SPI.setDataMode(SPI_MODE0); 
  SPI.transfer(0x15);
  SPI.transfer(0x55);
  SPI.transfer(0x40);
}
