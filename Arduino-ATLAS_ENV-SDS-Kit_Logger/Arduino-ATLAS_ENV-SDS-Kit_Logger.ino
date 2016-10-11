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
#include "RTClib.h"                 //Using RTC for timestamp
  RTC_DS1307 RTC;                   //Define RTC object
#include <SD.h>                     //Using SD card to log data 
  File SDdata;                      //Define SDdata object 
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
int measInterval = 10;             //Measuring interval in seconds
byte computer_bytes_received = 0;  //We need to know how many characters bytes have been received
byte sensor_bytes_received = 0;    //We need to know how many characters bytes have been received
char computerdata[20];             //20 byte character array to hold incoming data from computer
char sensordata[30];               //30 byte character array to hold incoming data from the sensors
  
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
  
  if (!SD.begin(SDPin)) {                                   //IF SD is not found
    Serial.println("SD not found.");                        //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println("successful.");


// CREATE DATA FILE
//---------------------------------------------------------  
  Serial.print("Creating data file: ");
  char filename[] = "DATA0000.CSV";                         //Create dummy filename 
  
  for (int i = 1; i < 10000; i++) {                         //FOR 1 to 10,000
    filename[4] = (i/1000)%10 + '0';                        //And create accordingly numbered filenames 
    filename[5] = (i/100)%10 + '0';                         //With ZERO as escape for char at end
    filename[6] = (i/10)%10 + '0';                                
    filename[7] = i%10 + '0';                          
    if (!SD.exists(filename)) {                             //IF filename does not exists  
      SDdata = SD.open(filename, FILE_WRITE);               //Name the file and make it writable  
      break;                                                //And leave the loop  
    }                                                       //End IF    
  }                                                         //End FOR    
  
  if (!SDdata) {                                            //IF data file object is not found
    Serial.println("Cannot create data file..");            //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  
  Serial.println("successful.");
  Serial.print("Logging to: ");
  Serial.println(filename);


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

void serialEvent() {                          //Trigger interrupt when data coming from serial monitor is received
  computer_bytes_received = Serial.readBytesUntil(13, computerdata, 20); 
                                              //Read data sent from serial monitor into "computerdata" until <CR> (ASCII char 13) and return number of characters received
  computerdata[computer_bytes_received] = 0;  //add NULL after last character received
}                                            //End Function SERIALEVENT
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
      Serial.print(sensordata);                       //And print measured data
    }                                                 //End FOR                  
    
    Serial.println();    
  }                                                   //End IF    


  

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

char *measurePort(int portNr) {
  digitalWrite(S1Pin, bitRead(portNr, 0));
  digitalWrite(S2Pin, bitRead(portNr, 1));
  digitalWrite(S3Pin, bitRead(portNr, 2));
  delay(1000);    
              
  altSerial.print("r");                             //Send the command from the computer to the Atlas Scientific device using the softserial port
  altSerial.print("\r");                            //After we send the command we send a carriage return <CR>
  delay(1000);    
             
  if (altSerial.available() > 0) {                 //If data has been transmitted from an Atlas Scientific device
    sensor_bytes_received = altSerial.readBytesUntil(13, sensordata, 30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received
    sensordata[sensor_bytes_received] = 0;         //Add "0" to array after last character received. This will stop us from transmitting incorrect data that may have been left in the buffer
  } //end IF
  delay(1000);
  return sensordata;         
}                                   //End MEASUREDATA
//==========================================================================================

