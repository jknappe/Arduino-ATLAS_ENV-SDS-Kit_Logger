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

#include <Wire.h>                   // connecting to RTC
#include "RTClib.h"                 // using RTC for timestamp
  RTC_DS1307 RTC;                   // define RTC object

int time = 0;
int plustime = 0;
  
//==========================================================================================


//==========================================================================================
// VOID SETUP
//------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);               //Set the hardware serial port to baud rate 9600  
  Wire.begin();                     //Start I2C communication

// SETTING RTC
//---------------------------------------  
  if (!RTC.begin()) {                                       //IF RTC in not running
    Serial.println("RTC not found.");                       //print error message
    while(1);                                               //and halt program
  }                                                         //End IF    
  
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));           //Initialize RTC with time of compilation
  Serial.print("RTC time is set to: ");
  printNowTime();                                           //Print current RTC time
  Serial.println();
  Serial.println("Recompile sketch to reset RTC to system time.");
  
}
//==========================================================================================


//==========================================================================================
// VOID LOOP
//------------------------------------------------------------------------------------------


void loop() {


            
}
//==========================================================================================


//==========================================================================================
// VOID CHAR NOWTIME
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
