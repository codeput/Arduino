// RTC control & report using serial monitor

#define CJ_ID "RTC_6b"
#define MONITOR Serial

#include <DS1307RTC.h>    // DS1307 library: returns time as a time_t
#include <Time.h>  
#include <Wire.h>         // I2C (SDA/SCL) for RTC communications

time_t t,timeX,timeWhenSet,timeC;

//// RTC non-volatile storage (not covered by time library!)
const int RTC_I2C_ADDR = 0x68;          // RTC I2C bus address, for accessing NV RAM
const byte RAM_ADDR   = 8;              // RTC RAM registers start at address 8
const byte TRIPA_ADDR = RAM_ADDR;       // Trip,ODO,lat,long not used.     
const byte TRIPB_ADDR = TRIPA_ADDR + 4; // not used - spacer only   
const byte ODO_ADDR   = TRIPB_ADDR + 4; // not used - spacer only
const byte LAT_ADDR   = ODO_ADDR   + 4; // not used - spacer only
const byte LON_ADDR   = LAT_ADDR   + 4; // not used - spacer only
const byte DRIFT_ADDR = LON_ADDR   + 4; // drift characteristic for this RTC in ms/hour (long)
const byte SET_ADDR   = DRIFT_ADDR + 4; // time when RTC was last set to UTC (time_t:long)

long longX, longY;                      // for temporary values
long drift_factor, drift;               // factor in ms/hour, drift in seconds

prog_char menu_0[]  PROGMEM = "COMMAND MENU:";   // "String 0" etc are strings to store - change to suit.
prog_char menu_1[]  PROGMEM = "      R: Reset RTC time";
prog_char menu_2[]  PROGMEM = "      S: Set RTC time";
prog_char menu_3[]  PROGMEM = "      +: drift factor +1";
prog_char menu_4[]  PROGMEM = "      -: drift factor -1";
prog_char menu_5[]  PROGMEM = "      W: Write drift_factor to NVRAM";
prog_char menu_6[]  PROGMEM = "      N: Report values in NVRAM";

prog_char str_0[]  PROGMEM = "--------- --------- --------- --------- ---------";
prog_char str_1[]  PROGMEM = "n/a";
prog_char str_2[]  PROGMEM = "n/a";
prog_char str_3[]  PROGMEM = "n/a";
prog_char str_4[]  PROGMEM = "n/a";
prog_char str_5[]  PROGMEM = "Status: timeNotSet";
prog_char str_6[]  PROGMEM = "Status: timeNeedsSync";
prog_char str_7[]  PROGMEM = "n/a";
prog_char str_8[]  PROGMEM = "**** Setting the time to: ";
prog_char str_9[]  PROGMEM = "**** Adjusting drift_factor by "; 

PROGMEM const char *menu_table[]={menu_0,menu_1,menu_2,menu_3,menu_4,menu_5,menu_6};
PROGMEM const char *str_table[] ={str_0,str_1,str_2,str_3,str_4,str_5,str_6,str_7,str_8,str_9};

char buffer[50];    // for the largest string plus NULL terminator

const int MAIN_LED = 13;

void setup(){
  MONITOR.begin(38400);
  pinMode(MAIN_LED, OUTPUT);
  digitalWrite(MAIN_LED,HIGH);
  printText(0,true);        // "--------- --------- --------- ---------"
  MONITOR.println(CJ_ID);
  setSyncProvider(RTC.get); // set RTC as master time source
  RTC_report_NVRAM();       // print values held in RTC NVRAM
  printText(0,true);        // "--------- --------- --------- ---------"
  printMenu(0,6);           // "COMMAND MENU ....
  printText(0,true);        // "--------- --------- --------- ---------"
  delay(1000);
  }

void loop() { 
  digitalWrite(MAIN_LED,!digitalRead(MAIN_LED));
  processSerialCommands();           
  timeX = now();                     // get Arduino time (based on RTC)
  if (timeStatus()==timeNotSet)    printText(5,true); // "Status: timeNotSet"
  if (timeStatus()==timeNeedsSync) printText(6,true); // "Status: timeNeedsSync"
  ReportTime(timeX,true,false);      // true,true = weekday/month as text, corrected time
  timeC = ComputeCorrectedTime(timeX);
  ReportTime(timeC,true,true);       // report corrected time
  delay(1000);
  printText(0,true);                 // "--------- --------- --------- ---------"
  }

// *******************************************************


void printMenu(int first, int last) {
  for (int i = first; i < last+1; i++) {
    strcpy_P(buffer, (char*)pgm_read_word(&(menu_table[i]))); // Necessary casts and dereferencing, just copy. 
    MONITOR.println(buffer);}  }

void processSerialCommands() {
  if( MONITOR.available() > 0) {
    char readChar = MONITOR.read();
    switch(readChar){
      case 'R': RTC_ResetTime();    break;
      case 'S': RTC_SetTime();      break;
      case '+': RTC_AdjustDrift(1);  break; // add 1 to drift_factor
      case '-': RTC_AdjustDrift(-1); break; // subtract 1 sec from drift_factor
      case 'W':                             // write drift_factor to NVRAM
        drift_factor = -250;                // RTC drift factor (ms/hr)
        MONITOR.print("Write: "); MONITOR.println(drift_factor); 
        RTC_WriteLong(DRIFT_ADDR, drift_factor); 
        break;
      case 'N':RTC_report_NVRAM(); break;
      default:  MONITOR.println("**** unrecognised command");}
    }
  }
  
void printText(int index, boolean linewrap) {
    strcpy_P(buffer, (char*)pgm_read_word(&(str_table[index]))); // Necessary casts and dereferencing, just copy. 
    if (linewrap) MONITOR.println(buffer); else MONITOR.print(buffer);
  }

void ReportTime(time_t t, boolean textreport, boolean reportCorrected){ 
  // textreport adds weekday and month as text
  if (reportCorrected) MONITOR.print("timeC: "); 
  else                 MONITOR.print("timeX: "); 
  MONITOR.print(timeX); MONITOR.print(" = ");
  if (textreport) {MONITOR.print(dayShortStr( weekday() ) ); MONITOR.print(" ");}
  printWithLeadingZero(hour(t)  ); MONITOR.print(":");
  printWithLeadingZero(minute(t)); MONITOR.print(":");
  printWithLeadingZero(second(t)); MONITOR.print(" ");
  printWithLeadingZero(day(t));
  if (textreport) {
    MONITOR.print(monthShortStr(month()));
    byte RTCshortyear = year(t) - 2000;
    MONITOR.print(RTCshortyear);}
  else {
    MONITOR.print("/");
    printWithLeadingZero(month(t));  MONITOR.print("/");
    printWithLeadingZero(year(t)); }
  if (reportCorrected) {MONITOR.print(" [+"); MONITOR.print(-drift); MONITOR.println(" secs]");}
  else MONITOR.println();
  }

time_t ComputeCorrectedTime(time_t t){ 
  // use 10^7 factor to avoid maths failure
  longY = (t - timeWhenSet)*(drift_factor*100/36); // (drift_factor*10000000/3600/1000)
  drift = longY/10000000;
  t = t - drift;
  return t;
  }

void RTC_ResetTime() {
  t = now();
  timeC = ComputeCorrectedTime(t);
  printText(8,false); MONITOR.println(timeC); // "**** Setting the time to: "
  setTime(timeC);                             // set the Arduino to corrected time
  RTC.set(timeC);                             // set the RTC to corrected time
  timeWhenSet = timeC;                      
  RTC_WriteLong(SET_ADDR, timeWhenSet);       // keep set time in NVRAM
  drift = 0;                                  // reset drift
  }

void RTC_SetTime() {
  //setTime(8,0,0,20,1,2012);                 // set the Arduino time to 8:00:00 20/Jan/12 
  setTime(11,32,50,11,2,2012);               
  timeWhenSet = now();                      // copy the Arduino time 
  RTC.set(timeWhenSet);                     // set the RTC to match
  RTC_WriteLong(SET_ADDR, timeWhenSet);     // keep set time in NVRAM 
  MONITOR.print("Time set to: "); MONITOR.println(timeWhenSet);
  }

void RTC_AdjustDrift(int delta){
  printText(9,false);                             // "**** Adjusting drift_factor by "
  MONITOR.println(delta);
  MONITOR.print("Drift was "); MONITOR.print(drift_factor); 
  //adjustTime(delta);
  drift_factor = drift_factor - delta; // +/- 1 sec
  RTC_WriteLong(DRIFT_ADDR, drift_factor); 
  MONITOR.print(" now "); MONITOR.println(drift_factor); 
  }
  
  // Read a 4-byte float value from the RTC RAM registers
float RTC_ReadFloat(int valAddr) {
  float value;
  byte  *byteArray = (byte *) &value;
  Wire.beginTransmission( RTC_I2C_ADDR );  // Set the register pointer
  Wire.write( valAddr );
  Wire.endTransmission( );
  // Read 4 bytes of float value
  Wire.requestFrom( RTC_I2C_ADDR, 4 );
  byteArray[3] = Wire.read( );
  byteArray[2] = Wire.read( );
  byteArray[1] = Wire.read( );
  byteArray[0] = Wire.read( );
  return value;}
  
// Write a 4-byte float value to the RTC RAM registers
void RTC_WriteFloat(int valAddr, float value){
  if(valAddr > 7 && valAddr < 61 ) { // Don't let writes go to the RTC registers 0 - 7
    Wire.beginTransmission( RTC_I2C_ADDR );
    Wire.write(valAddr);
    // Write high word (high byte/low byte), low word (high byte/low byte)
    byte  *byteArray;
    byteArray = (byte *) &value;
    Wire.write(byteArray[3] );
    Wire.write(byteArray[2] );
    Wire.write(byteArray[1] );
    Wire.write(byteArray[0] );
    Wire.endTransmission( );} }
  
// Read a 4-byte long value from the RTC RAM registers
long RTC_ReadLong (int valAddr) {
  long value;
  byte  *byteArray = (byte *) &value;
  Wire.beginTransmission(RTC_I2C_ADDR);  // Set the register pointer
  Wire.write(valAddr);
  Wire.endTransmission( );
  // Read 4 bytes of float value
  Wire.requestFrom(RTC_I2C_ADDR,4);
  byteArray[3] = Wire.read();
  byteArray[2] = Wire.read();
  byteArray[1] = Wire.read();
  byteArray[0] = Wire.read();
  return value;}
  
// Write a 4-byte long value to the RTC RAM registers
void RTC_WriteLong(int valAddr, long value){
  if(valAddr > 7 && valAddr < 61 ) { // Don't let writes go to the RTC registers 0 - 7
    Wire.beginTransmission(RTC_I2C_ADDR);
    Wire.write(valAddr);
    // Write high word (high byte/low byte), low word (high byte/low byte)
    byte  *byteArray = (byte *) &value;
    //byteArray = (byte *) &value;
    Wire.write(byteArray[3] );
    Wire.write(byteArray[2] );
    Wire.write(byteArray[1] );
    Wire.write(byteArray[0] );
    Wire.endTransmission( );} }

void RTC_report_NVRAM(){
        drift_factor    = RTC_ReadLong(DRIFT_ADDR); // RTC drift ms/hour
        timeWhenSet = RTC_ReadLong(SET_ADDR);   // time when RTC last set to UTC
        //f_ODO  = RTC_ReadFloat(ODO_ADDR); // Odometer 
        //valODO = f_ODO;
        MONITOR.print("NVRAM: "); MONITOR.print(drift_factor); 
              MONITOR.print("/"); MONITOR.print(timeWhenSet); 
            //MONITOR.print("/"); MONITOR.print(valODO);
              MONITOR.println();
  }
  
void printWithLeadingZero(int digits){
  if(digits < 10) MONITOR.print('0'); MONITOR.print(digits);}
