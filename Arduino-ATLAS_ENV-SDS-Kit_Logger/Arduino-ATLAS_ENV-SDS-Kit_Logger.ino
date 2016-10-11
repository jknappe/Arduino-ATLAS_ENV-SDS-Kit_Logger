//===========================================================================================
// PREAMBLE
//-------------------------------------------------------------------------------------------

//Logger type: Arduino UNO with Adafruit Datalogging Shield
 
//Program author: Jan Knappe
//Contact: jan.knappe@tcd.ie

//===========================================================================================

//===========================================================================================
// SETUP
//-------------------------------------------------------------------------------------------

// INCLUDE LIBRARIES
//---------------------------------------------------------  
#include <Wire.h>                   //Connecting to RTC
#include <RTClib.h>                 //Using RTC for timestamp
  RTC_DS1307 RTC;                   //Define RTC object
#include <SPI.h>                    //COMMENTING NEEDED
#include <SdFat.h>                  //COMMENTING NEEDED
  SdFat sd;                         //COMMENTING NEEDED
  SdFile file;                      //COMMENTING NEEDED
#include <AltSoftSerial.h>          //Using software serials on UNO: TX 9, RX 8, unusable 10
  AltSoftSerial altSerial;          //Define altSerial object

// DEFINE PINS
//---------------------------------------------------------  
const byte LEDPin = 2;              //LED pin 
const byte SDPin = 10;              //SD shield CS pin (10 on datalogging shield, 53 on MEGA)
const byte S1Pin = 7;               //Arduino pin 31 to control pin S1
const byte S2Pin = 6;               //Arduino pin 32 to control pin S2
const byte S3Pin = 5;               //Arduino pin 33 to control pin S3

// DEFINE VARIABLES
//---------------------------------------------------------  
int measInterval = 10;              //Measuring interval in seconds
byte loggerBytesSent = 0;           //Length of bytes sent from logger
byte sensorBytesReceived = 0;       //Length of bytes received from sensors
char loggerCommand[20];             //Array to hold outcoming data from logger
char sensorData[30];                //Array to hold incoming data from sensors
  
//==========================================================================================


//==========================================================================================
// VOID SETUP
//------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);               //Set the hardware serial port to baud rate 9600  
  Wire.begin();                     //Start I2C communication


// SETTING RTC
//---------------------------------------------------------  
  Serial.print("Initializing RTC: ");
  
  if (!RTC.begin()) {                                       //IF RTC is not found
    Serial.println("RTC not found.");                       //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println("successful"); 
  
  if (!RTC.isrunning()) {                                   //IF RTC is not running
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));         //Initialize RTC with time of compilation
  }                                                         //End IF    
  
  Serial.print("RTC time is set to: ");
  printNowTime();                                           //Print current RTC time
  Serial.println();
  Serial.println("To reset RTC to system time, change battery and compile sketch.");


// SETTING SD CARD
//---------------------------------------------------------  
  pinMode(SDPin, OUTPUT);                                   //Set SDPin as output
    digitalWrite(SDPin, HIGH);                              //Activate internal pullup resistor
  Serial.print("Initializing SD Card: ");
  
  if (!sd.begin(SDPin, SPI_HALF_SPEED)) {                   //IF SD is not found
    Serial.println("SD not found.");                        //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println("successful.");


// CREATE DATA FILE
//---------------------------------------------------------  
  Serial.print("Creating data file: ");
  char fileName[] = "DATA0000.CSV";                         //Create dummy filename 
  
  for (int i = 1; i < 10000; i++) {                         //FOR 1 to 10,000
    fileName[4] = (i/1000)%10 + '0';                        //And create accordingly numbered filenames 
    fileName[5] = (i/100)%10 + '0';                         //And add NULL as escape character
    fileName[6] = (i/10)%10 + '0';                                
    fileName[7] = i%10 + '0';                          
    if (!sd.exists(fileName)) {                             //IF filename does not exists  
    //  file = sd.open(fileName, FILE_WRITE);               //Name the file and make it writable  
      break;                                                //And leave the loop  
    }                                                       //End IF    
  }                                                         //End FOR    
  
  if (!file.open(fileName, O_CREAT | O_WRITE)) {            //COMMENTING NEEDED
    Serial.println("Cannot create data file..");            //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF
  
  //if (!file) {                                              //IF data file object is not found
  //  Serial.println("Cannot create data file..");            //Print error message
  //  while(1);                                               //And halt program
  //}                                                         //End IF    
  
  Serial.println("successful.");
  Serial.print("Logging to: ");
  Serial.println(fileName);


// INITIALIZE PORT MULTIPLiER
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

// SET FLAG FOR MEASURING
//---------------------------------------------------------  

  if ( (millis()/1000)%measInterval == 0) {           //IF seconds since starts matches measInterval

    printNowTime();                                   //Print timestamp
    
    for (int portNr = 0; portNr < 5; portNr ++) {     //FOR 0 to 4  
      measurePort(portNr);                            //Measure port
      Serial.print(",");             
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
  Serial.print("/");                //Print separator   
  Serial.print(now.year(), DEC);    //Print year   
  Serial.print(" ");                //Print separator
  if (now.hour() < 10){             //IF hour < 10
    Serial.print(0);                //Print leading zero
  }                                 //End IF
  Serial.print(now.hour(), DEC);    //Print hour
  Serial.print(":");                //Print separator
  if (now.minute() < 10){           //IF minute < 10
    Serial.print(0);                //Print leading zero;
  }                                 //End IF
  Serial.print(now.minute(), DEC);  //Print minute
  Serial.print(":00");              //Print separator and seconds
  
}                                   //End VOID PRINTNOWTIME
//==========================================================================================


//==========================================================================================
// FUNCTION MEASUREPORT
//------------------------------------------------------------------------------------------

char *measurePort(int portNr) {                                           //To measure ports on port multiplier
  
  digitalWrite(S1Pin, bitRead(portNr, 0));                                //Convert portNr into binary on S-ports
  digitalWrite(S2Pin, bitRead(portNr, 1));
  digitalWrite(S3Pin, bitRead(portNr, 2));
  delay(100);                                                             //Wait for switch of ports  
              
  altSerial.print("r");                                                   //Send measure command from serial to multiplier
  altSerial.print("\r");                                                  //Send carriage return <CR> as ENTER
  delay(1000);                                                            //Wait for sensor stabilization (TO DO: NEEDS SENSOR SPECIFIC ADJUSTMENT)
             
  if (altSerial.available() > 0) {                                        //IF data was transmitted from multiplier
    sensorBytesReceived = altSerial.readBytesUntil(13, sensorData, 30);   //Read data and count characters received
    sensorData[sensorBytesReceived] = 0;                                  //And add NULL as escape character
  }                                                                       //End IF
  
  delay(100);
  return sensorData;         
}                                                                        //End MEASUREDATA
//==========================================================================================

