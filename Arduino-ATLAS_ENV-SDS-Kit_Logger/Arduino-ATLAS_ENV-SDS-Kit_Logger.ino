//===========================================================================================
// PREAMBLE
//-------------------------------------------------------------------------------------------

//Logger type: Arduino MEGA with RTC and SD shield
 
//Program author: Jan Knappe
//Contact: jan.knappe@tcd.ie

//===========================================================================================

//===========================================================================================
// SETUP
//-------------------------------------------------------------------------------------------

// INCLUDE LIBRARIES
//---------------------------------------------------------  
#include <Wire.h>                           //Connecting to RTC
#include <RTClib.h>                         //Using RTC for timestamp
  RTC_DS1307 RTC;                           //Define RTC object
#include <SPI.h>                            //Enable Serial Peripheral Interface communication
#include <SdFat.h>                          //Communicate with SD card
  SdFat sd;                                 //Defining SdFat object
  SdFile file;                              //Defining SdFile object
#include <AltSoftSerial.h>                  //Using software serials on MEGA: TX 46, RX 48, unusable 44,45
  AltSoftSerial altSerial;                  //Define altSerial object
#include <SoftwareSerial.h>                 //Access software serial (for BT communication)
  SoftwareSerial bluetoothSerial(11, 12);   //Define RX, TX for BT communication

// DEFINE PINS
//---------------------------------------------------------  
const byte LEDPin = 41;             //LED pin 
const byte SDPin = 53;              //SD shield CS pin (10 on datalogging shield, 53 on MEGA)
const byte S1Pin = 31;              //Arduino pin 31 to control pin S1
const byte S2Pin = 32;              //Arduino pin 32 to control pin S2
const byte S3Pin = 33;              //Arduino pin 33 to control pin S3

// DEFINE VARIABLES
//---------------------------------------------------------  
int measInterval = 10;              //Measuring interval in seconds
byte loggerBytesSent = 0;           //Length of bytes sent from logger
byte sensorBytesReceived = 0;       //Length of bytes received from sensors
char loggerCommand[20];             //Array to hold outcoming data from logger
char sensorData[30];                //Array to hold incoming data from sensors
char bluetoothSend;                 //Variable to receive BT data
  
//==========================================================================================


//==========================================================================================
// VOID SETUP
//------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);               //Set the hardware serial port to baud rate 9600  
  Wire.begin();                     //Start I2C communication


// SETTING RTC
//---------------------------------------------------------  
  Serial.print(F("Initializing RTC: "));
  
  if (!RTC.begin()) {                                       //IF RTC is not found
    Serial.println(F("RTC not found."));                    //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println(F("successful")); 
  
  if (!RTC.isrunning()) {                                   //IF RTC is not running
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));         //Initialize RTC with time of compilation
  }                                                         //End IF    
  
  Serial.print(F("RTC time is set to: "));
  printNowTime();                                           //Print current RTC time
  Serial.println();
  Serial.println(F("To reset RTC to system time, change battery and compile sketch."));


// INITIALIZE BLUETOOTH CONNECTION
//---------------------------------------------------------  
  bluetoothSerial.begin(9600);                              //Set BT serial port to baud rate 9600
  bluetoothSerial.println("Bluetooth connection established.");

  Serial1.begin(9600);


// SETTING SD CARD
//---------------------------------------------------------  
  pinMode(SDPin, OUTPUT);                                   //Set SDPin as output
    digitalWrite(SDPin, HIGH);                              //Activate internal pullup resistor
  Serial.print(F("Initializing SD Card: "));
  
  if (!sd.begin(SDPin, SPI_HALF_SPEED)) {                   //IF SD is not found
    Serial.println(F("SD not found."));                     //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println(F("successful."));


// CREATE DATA FILE
//---------------------------------------------------------  
  Serial.print(F("Creating data file: "));
  char fileName[] = "DATA0000.CSV";                         //Create dummy filename 
  
  for (int i = 1; i < 10000; i++) {                         //FOR 1 to 10,000
    fileName[4] = (i/1000)%10 + '0';                        //And create accordingly numbered filenames 
    fileName[5] = (i/100)%10 + '0';                         //And add NULL as escape character
    fileName[6] = (i/10)%10 + '0';                                
    fileName[7] = i%10 + '0';                          
    if (!sd.exists(fileName)) {                             //IF filename does not exists  
      break;                                                //And leave the loop  
    }                                                       //End IF    
  }                                                         //End FOR    
  
  if (!file.open(fileName, O_CREAT | O_WRITE)) {            //IF file cannot be created or opened for writing
    Serial.println(F("Cannot create data file.."));         //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF
    
  Serial.println(F("successful."));
  Serial.print(F("Logging to: "));
  Serial.println(fileName);


// INITIALIZE PORT MULTIPLEXER
//---------------------------------------------------------  
  pinMode(S1Pin, OUTPUT);                                   //Set pin as output
  pinMode(S2Pin, OUTPUT);                                   //Set pin as output
  pinMode(S3Pin, OUTPUT);                                   //Set pin as output   

  altSerial.begin(9600);                                    //Set the soft serial port to baud rate 9600


// BLINK AT START
//---------------------------------------------------------  
  pinMode(LEDPin, OUTPUT);                                  //LED as output  
  
  for (int i = 1; i < 4; i ++) {                            //FOR 1 to 3
    digitalWrite(LEDPin, HIGH);                             //Blink LED on/off
    delay(500);                                             //To indicate start of logging
    digitalWrite(LEDPin, LOW);
    delay(500);                                 
  }                                                         //End FOR    
  
}                                                           //End VOID SETUP
//==========================================================================================


//===========================================================================================
// FUNCTION SERIALEVENT
//-------------------------------------------------------------------------------------------

void serialEvent() {                                              //Trigger interrupt for incoming data from serial
  
  loggerBytesSent = Serial.readBytesUntil(13, loggerCommand, 20); //Read data from serial and return number of characters received
  loggerCommand[loggerBytesSent] = 0;                             //And add NULL as escape character
  
}                                                                 //End Function SERIALEVENT
//===========================================================================================


//==========================================================================================
// VOID LOOP
//------------------------------------------------------------------------------------------


void loop() {

// BLUETOOTH
//---------------------------------------------------------  
   
  if (Serial1.available()) {                           //COMMENTING NEEDED
    int inByte = Serial1.read();
    Serial.write(inByte);
  }

  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
   Serial1.println(millis());
  

// SET FLAG FOR MEASURING
//---------------------------------------------------------  

  if ( (millis()/1000)%measInterval == 0) {           //IF seconds since starts matches measInterval

    printNowTime();                                   //Print timestamp
    
    for (int portNr = 0; portNr < 5; portNr ++) {     //FOR 0 to 4  
      measurePort(portNr);                            //Measure port
      Serial.print(F(","));             
      Serial.print(sensorData);                       //And print measured data
    }                                                 //End FOR                  
    
    Serial.println();    
  }                                                   //End IF    


  
  file.close();                                       //Close file on SD card

}                                                     //End VOID LOOP
//==========================================================================================


//==========================================================================================
// FUNCTION PRINTNOWTIME
//------------------------------------------------------------------------------------------

void printNowTime() {               //To print current RTC time in DD/MM/YYYY HH:MM:SS format
  
  DateTime now = RTC.now();         //Fetch time from RTC

  if (now.day() < 10){              //IF days < 10
    Serial.print(0);                //Print leading zero
  }                                 //End IF
  Serial.print(now.day(), DEC);     //Print day        
  Serial.print("/");                //Print separator  
  if (now.month() < 10){            //IF month < 10
    Serial.print(0);                //Print leading zero
  }                                 //End IF
  Serial.print(now.month(), DEC);   //Print month   
  Serial.print(F("/"));             //Print separator   
  Serial.print(now.year(), DEC);    //Print year   
  Serial.print(F(" "));             //Print separator
  if (now.hour() < 10){             //IF hour < 10
    Serial.print(0);                //Print leading zero
  }                                 //End IF
  Serial.print(now.hour(), DEC);    //Print hour
  Serial.print(F(":"));             //Print separator
  if (now.minute() < 10){           //IF minute < 10
    Serial.print(0);                //Print leading zero;
  }                                 //End IF
  Serial.print(now.minute(), DEC);  //Print minute
  Serial.print(F(":00"));           //Print separator and seconds
  
}                                   //End VOID PRINTNOWTIME
//==========================================================================================


//==========================================================================================
// FUNCTION MEASUREPORT
//------------------------------------------------------------------------------------------

char *measurePort(int portNr) {                                           //To measure ports on port multiplexer
  
  digitalWrite(S1Pin, bitRead(portNr, 0));                                //Convert portNr into binary on S-ports
  digitalWrite(S2Pin, bitRead(portNr, 1));
  digitalWrite(S3Pin, bitRead(portNr, 2));
  delay(100);                                                             //Wait for switch of ports  
              
  altSerial.print("r");                                                   //Send measure command from serial to multiplexer
  altSerial.print("\r");                                                  //Send carriage return <CR> as ENTER
  delay(1000);                                                            //Wait for sensor stabilization (TO DO: NEEDS SENSOR SPECIFIC ADJUSTMENT)
             
  if (altSerial.available() > 0) {                                        //IF data was transmitted from multiplexer
    sensorBytesReceived = altSerial.readBytesUntil(13, sensorData, 30);   //Read data and count characters received
    sensorData[sensorBytesReceived] = 0;                                  //And add NULL as escape character
  }                                                                       //End IF
  
  delay(100);
  return sensorData;         
}                                                                        //End MEASUREDATA
//==========================================================================================

