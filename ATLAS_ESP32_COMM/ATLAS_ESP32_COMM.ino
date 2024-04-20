#include <SoftwareSerial.h>
// software serial #1: RX = digital pin 16, TX = digital pin 17
SoftwareSerial portOne(16, 17);

// software serial #2: RX = digital pin 18, TX = digital pin 19

SoftwareSerial portTwo(18, 19);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  portOne.begin(115200);
  portTwo.begin(115200);
}

void loop() {
  ///////////  SEND    /////////////
  //---> This code here should send commands
  SendCommand(1, ke);
  ///////////  RECIEVE /////////////
  portTwo.listen();
  Serial.println();
  while (portTwo.available() > 0){
    char inByte = portTwo.read();
    Serial.println(inByte);
    
   //We should recieve here the Measurement data from the Nano 
   // and Process them into Pressure and Depth data 
  }
}

void SendCommand(int num, char k){
  for(int counter = 0;counter < num;counter++){
    PortOne.println(k);
  }
}
