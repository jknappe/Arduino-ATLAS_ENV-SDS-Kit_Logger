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
  
//==========================================================================================


//==========================================================================================
// VOID SETUP
//------------------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(9600);               //Set the hardware serial port to baud rate 9600  
  Wire.begin();                     //Start I2C communication

  
  if (!RTC.begin()) {                                       //IF RTC in not running
    Serial.println("RTC not found.");                       //print error message
    while(1);                                               //and halt program
  }                                                         //End IF    
  
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));           //initialize RTC with time of compilation
  Serial.print("RTC time is set to:");
  Serial.println();
  
}

//==========================================================================================


//==========================================================================================
// VOID LOOP
//------------------------------------------------------------------------------------------


void loop() {

    DateTime now = RTC.now();                     // fetch time from RTC

      if (now.day() < 10){   // Add a zero, if necessary
        Serial.print(0);
      }
      Serial.print(now.day(), DEC);        
      Serial.print("/");  
     if (now.month() < 10){   // Add a zero, if necessary
        Serial.print(0);
      }
      Serial.print(now.month(), DEC);   
      Serial.print("/");   
      Serial.print(now.year(), DEC);   
      Serial.print(" ");
      if (now.hour() < 10){   // Add a zero, if necessary
        Serial.print(0);
      }
        Serial.print(now.hour(), DEC);
        Serial.print(":");
      if (now.minute() < 10){   // Add a zero, if necessary
         Serial.print(0);
      }
      Serial.print(now.minute(), DEC);
      Serial.print(":");
      if (now.second() < 10){   // Add a zero, if necessary
        Serial.print(0);
      }
      Serial.print(now.second(), DEC);
      Serial.println();
      delay(1000);

        
}

//==========================================================================================


//==========================================================================================
// VOID LOOP
//------------------------------------------------------------------------------------------



//==========================================================================================
