// DS1302_Serial_Hard (C)2010 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// A quick demo of how to use my DS1302-library to 
// retrieve time- and date-date for you to manipulate.
//
// I assume you know how to connect the DS1302.
// DS1302:  CE pin    -> Arduino Digital 2
//          I/O pin   -> Arduino Digital 3
//          SCLK pin  -> Arduino Digital 4

#include <DS1302.h>
#include "SoftwareSerial.h"
const int Rx = 9; // this is physical pin 2
const int Tx = 10; // this is physical pin 3
SoftwareSerial mySerial(Rx, Tx);
// Init the DS1302
DS1302 rtc(2, 3, 4);

// Init a Time-data structure
Time t;

void setup()
{
  // Set the clock to run-mode, and disable the write protection
  rtc.halt(false);
  rtc.writeProtect(false);
  mySerial.begin(9600);

  // The following lines can be commented out to use the values already stored in the DS1302
  rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY
  rtc.setTime(12, 0, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(6, 8, 2010);   // Set the date to August 6th, 2010
}

void loop()
{
  // Get data from the DS1302
  t = rtc.getTime();
 // Send date over serial connection
  mySerial.print("Today is the ");
  mySerial.print(t.date, DEC);
  mySerial.print(". day of ");
  mySerial.print(rtc.getMonthStr());
  mySerial.print(" in the year ");
  mySerial.print(t.year, DEC);
  mySerial.println(".");
  
  // Send Day-of-Week and time
  mySerial.print("It is the ");
  mySerial.print(t.dow, DEC);
  mySerial.print(". day of the week (counting monday as the 1th), and it has passed ");
  mySerial.print(t.hour, DEC);
  mySerial.print(" hour(s), ");
  mySerial.print(t.min, DEC);
  mySerial.print(" minute(s) and ");
  mySerial.print(t.sec, DEC);
  mySerial.println(" second(s) since midnight.");

  // Send a divider for readability
  mySerial.println("  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -");
  
  // Wait one second before repeating :)
  delay (1000); 
} 


