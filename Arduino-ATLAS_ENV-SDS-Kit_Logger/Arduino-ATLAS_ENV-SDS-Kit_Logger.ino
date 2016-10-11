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
//---------------------------------------  
#include <Wire.h>                   //Connecting to RTC
#include "RTClib.h"                 //Using RTC for timestamp
  RTC_DS1307 RTC;                   //Define RTC object
#include <SD.h>                     //Using SD card to log data 
  File SDdata;                      //Define SDdata object 

// DEFINE PINS
//---------------------------------------  
const byte SDPin = 10;              //SD shield CS pin (10 on datalogging shield, 53 on MEGA)

// DEFINE VARIABLES
//---------------------------------------  

  
//==========================================================================================


//==========================================================================================
// VOID SETUP
//------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);               //Set the hardware serial port to baud rate 9600  
  Wire.begin();                     //Start I2C communication

// SETTING RTC
//---------------------------------------  
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
  Serial.println("Change battery and compile sketch to reset RTC to system time.");

// SETTING SD CARD
//---------------------------------------    
  pinMode(SDPin, OUTPUT);                                   //Set SDPin as output
    digitalWrite(SDPin, HIGH);                              //Activate internal pullup resistor
  Serial.print("Initializing SD Card: ");
  
  if (!SD.begin(SDPin)) {                                   //IF SD is not found
    Serial.println("SD not found.");                        //Print error message
    while(1);                                               //And halt program
  }                                                         //End IF    
  Serial.println("successful.");
  
}
//==========================================================================================


//==========================================================================================
// VOID LOOP
//------------------------------------------------------------------------------------------


void loop() {


            
}
//==========================================================================================


//==========================================================================================
// VOID PRINTNOWTIME
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
  Serial.print(":");                //Print separator
  if (now.second() < 10){           //IF second < 10
    Serial.print(0);                //Print leading zero
  }                                 //End IF
  Serial.print(now.second(), DEC);  //Print second
  
}
//==========================================================================================
