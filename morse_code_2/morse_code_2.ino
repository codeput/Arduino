//
String navn = ("CW BY OZ6YM -");
//
//
//
///////////////////////////////////////////////////////////////////////
// CW Decoder made by Hjalmar Skovholm Hansen OZ1JHM  VER 1.02       //
// Feel free to change, copy or what ever you like but respect       //
// that license is http://www.gnu.org/copyleft/gpl.html              //
// Discuss and give great ideas on                                   //
// https://groups.yahoo.com/neo/groups/oz1jhm/conversations/messages //
///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////
// Read more here http://en.wikipedia.org/wiki/Goertzel_algorithm        //
// if you want to know about FFT the http://www.dspguide.com/pdfbook.htm //
///////////////////////////////////////////////////////////////////////////
// *******************************************************
// ** The DECODETABLE is a part of the decode-routine   ** 
// ** writen by OZ6YM, Palle http://www.planker.dk      **
// ** February 2015                                     **
// ** Compile: 7.972 bytes (af en 32.256 byte maksimum  **
// ** Some part is deleted as not nessesary in the prog.**
// ** all for speeding up the program.                  **
// ** FreqCount.h is add to measure decoder frequens    **
// *******************************************************
/*
 #include <Wire.h>
 #include <LCD.h>
 #include <LiquidCrystal_I2C.h>

 #define I2C_ADDR 0x27                        // Define I2C Address where the PCF8574A is
 #define BACKLIGHT_PIN 3
 #define BACKLIGHT_POL POSITIVE
 #define En_pin 2
 #define Rw_pin 1
 #define Rs_pin 0
 #define D4_pin 4
 #define D5_pin 5
 #define D6_pin 6
 #define D7_pin 7
 LiquidCrystal_I2C lcd (I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BACKLIGHT_PIN, BACKLIGHT_POL);
*/
#include <LiquidCrystal.h>
//#include <FreqCount.h>
// We cane NOT have both "tone" in port 12 and a frequens-counter implemented at the same time
// so have to chose one or the other.
//********************************************************************************************

/*
FreqCount.h:
------------
Board         Freq. InputPin       Pins Unusable with analogWrite()
-------------------------------------------------------------------
Arduino Uno |       5        |            3, 9, 10, 11 
-------------------------------------------------------------------
*/


///////////////////////////////////////////////
// select the pins used on the LCD panel      /
// **********************************************
// ** LiquidCrystal lcd(RS, E, D4, D5, D6, D7) **
// **********************************************
 LiquidCrystal lcd(2, 3, 5, 6, 7, 8 );
// **********************************************
// ***      END of pin- assignment             **
// **********************************************

// ********** Defining the DISPLAY **************
// ** Display is 20 character pr line          **
// ** Display have 4 lines                     **
// **********************************************
const int colums = 16;             // have to be 16 or 20, could be other size
const int rows = 2;                // have to be 2 or 4
// **********************************************
// *** END of DISPLAY definition               **
// **********************************************

// **********************************************
// **  Variabel definitions for Display use    **
// **********************************************
int lcdindex = 0;
int line1[colums];
int line2[colums];
// **********************************************
// *** END of Variaberl definition             **
// **********************************************

////////////////////////////////
// Define 8 specials letters  //
////////////////////////////////
byte U_umlaut[8] =   {B01010,B00000,B10001,B10001,B10001,B10001,B01110,B00000}; // ' '  
byte O_umlaut[8] =   {B01010,B00000,B01110,B10001,B10001,B10001,B01110,B00000}; // ' '   
byte A_umlaut[8] =   {B01010,B00000,B01110,B10001,B11111,B10001,B10001,B00000}; // ' '      
byte AE_capital[8] = {B01111,B10100,B10100,B11110,B10100,B10100,B10111,B00000}; // ' '   
byte OE_capital[8] = {B00001,B01110,B10011,B10101,B11001,B01110,B10000,B00000}; // ' '  
byte fullblock[8] =  {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};  
byte AA_capital[8] = {B00100,B00000,B01110,B10001,B11111,B10001,B10001,B00000}; // ' '   
byte emtyblock[8] =  {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000}; 
// **********************************************
// ** END of Special Letters                   **
// **********************************************

// **********************************************
// ** Other port assignments on ARDUINO UNO    **
// ** Read the application note if you use a   **
// ** ATmega328p at: http://www.atmel.com/images/Atmel-8271-8-bit-AVR-Microcontroller-ATmega48A-48PA-88A-88PA-168A-168PA-328-328P_datasheet_Complete.pdf
// **********************************************
int audioInPin = A3;   // ARDUINO UNO-port AD1 - This is the port READING signal from radio.
//int analogPin = A5;    // Frekvens-tæller
int audioOutPin = 10;   // ARDUINO tone on pin 0 - ATmega328P, pin 2 =PD0(PCINT16/RXD) - A 8 Ohm lautspeaker in serie with a 100 Ohm Resistor make received CW-sound 
int ledPin = 13;       // Lightdiode-port 13      - ATmega328P, pin 19=PB5 (PCINT5/SCK ) - A light-diode on this port will blink in same tact af received morse-sign
// **********************************************
// *** END of Other port assignments          **
// **********************************************

// **********************************************
// **   Program Variable                       **
// **********************************************
float magnitude ;
int magnitudelimit = 100;
int magnitudelimit_low = 100;
int realstate = LOW;
int realstatebefore = LOW;
int filteredstate = LOW;
int filteredstatebefore = LOW;
///////////////////////////////////////////////////////////////////////////////////////////////////
// The sampling frq will be 8928 on a 16 mhz without any prescaler etc because we need the tone  //
// in the center of the bins you can set the tone to 496, 558, 744 or 992 then n the number of   //
// samples which give the bandwidth can be (8928 / tone) * 1 or 2 or 3 or 4 etc init is          //
// 8928/558 = 16 *4 = 64 samples try to take n = 96 or 128 ;o)                                   //
//               48 will give you a bandwidth around 186 hz                                      //
//               64 will give you a bandwidth around 140 hz                                      //
//               96 will give you a bandwidth around 94 hz                                       //
//              128 will give you a bandwidth around 70 hz                                       //
// BUT remember that high n take a lot of time so you have to find the compromice - i use 48     //
///////////////////////////////////////////////////////////////////////////////////////////////////
float coeff;
float Q1 = 0;
float Q2 = 0;
float sine;
float cosine;  
float sampling_freq=8928.0;
float target_freq=700.0;                   /// adjust for your needs see above
float n=48.0;                              //// if you change  her please change next line also 
int testData[48];
// ************************************************************************************************
///////////////////////////////////////////////////////////////////
// Noise Blanker time which shall be computed so this is initial //
///////////////////////////////////////////////////////////////////
int nbtime = 6;  /// ms noise blanker         
long starttimehigh;
long highduration;
long lasthighduration;
long hightimesavg;
long lowtimesavg;
long startttimelow;
long lowduration;
long laststarttime = 0;
char code[20];
int stop = LOW;
int wpm;
unsigned long count;
int PalStop = 0;
int countMag = 0;

// *************** Revrited february 2015 *****************
// **           by OZ6YM, Palle A. Andersen              **
// **              http://www.planker.dk                 **
// ********************************************************
// **                  DECODE-TABLE                      **
// ********************************************************
char const VARcw0[47][9] = {"--..--","-....-",".-.-.-","-..-.","-----",".----","..---","...--","....-",".....","-....","--...","---..","----.","---...","-.-.-.",
".-.-.","-...-","-.-.--","..--..",".--.-.",".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",".--.","--.-",".-.","...",
"-","..-","...-",".--","-..-","-.--","--.."};
// ***************************************************************************************************************************************************
// **  ASCII-karakterer linear from no: 44 T.O.M. 90                                                                                                **
// **  44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90 **
// ***************************************************************************************************************************************************
char const VARcw1[8][9] = {"..--.", "...-..-", "-.--.-", "-.--.", "e", ".-.-", "---.", ".--.-"};
// *******************************************************
// **  ASCII-Charakter outside the lineare numbers      **
// *******************************************************
int VARchr[8] = {33,36,40,41,0,3,4,6}; 
// *******************************************************
// **         END of re-writed code de OZ6YM            **
// *******************************************************

////////////////
// init setup //
////////////////
void setup() 
{
//    FreqCount.begin(1000);
  ////////////////////////////////////
  // The basic goertzel calculation //
  ////////////////////////////////////
  int  k;
  float omega;
  k = (int) (0.5 + ((n * target_freq) / sampling_freq));
  omega = (2.0 * PI * k) / n;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;

  ///////////////////////////////
  // define special characters //
  ///////////////////////////////
  lcd.createChar(0, U_umlaut); //     German
  lcd.createChar(1, O_umlaut); //     German, Swedish
  lcd.createChar(2, A_umlaut); //     German, Swedish 
  lcd.createChar(3, AE_capital); //   Danish, Norwegian
  lcd.createChar(4, OE_capital); //   Danish, Norwegian
  lcd.createChar(5, fullblock);        
  lcd.createChar(6, AA_capital); //   Danish, Norwegian, Swedish
  lcd.createChar(7, emtyblock); 
  lcd.clear(); 

// *******************************************************************************
 Serial.begin(115200); // All serial communication is only used in DEBUG mode...
// *******************************************************************************
  pinMode(ledPin, OUTPUT);
  lcd.begin(colums, rows); 
  for (int index = 0; index < colums; index++){
    line1[index] = 32;
    line2[index] = 32;
  }  
//lcd.backlight(); // Backlight on  
}
// ************************************************
// ** END of SETUP                               **
// ************************************************

///////////////
// main loop //
///////////////
void loop() 
{
 /* if (FreqCount.available()) 
    {
     count = FreqCount.read();

     target_freq = (count );
          Serial.println(target_freq);
    }*/ 
  ///////////////////////////////////// 
  // The basic where we get the tone //
  /////////////////////////////////////
  for (char index = 0; index < n; index++)
  {
    testData[index] = analogRead(audioInPin);
  }
  for (char index = 0; index < n; index++)
  {
    float Q0;
    Q0 = coeff * Q1 - Q2 + (float) testData[index];
    Q2 = Q1;
    Q1 = Q0;  
  }
  float magnitudeSquared = (Q1*Q1)+(Q2*Q2)-Q1*Q2*coeff;  // we do only need the real part //
  magnitude = sqrt(magnitudeSquared);
  Q2 = 0;
  Q1 = 0;
  // *********************************************************************************************
  /*
  PalStop++;   // here you can measure magnitude for setup..
  
    if ( PalStop >= 25 ) 
    { 
      PalStop =0;
      lcd.setCursor(0,0); lcd.print("              ");lcd.setCursor(0,0);
      for ( int i=0; i<=countMag;i++)
        { 
          lcd.write(255); 
        }
      countMag = 0;
    }
    else
    {
      countMag = (countMag + int(magnitude))/ PalStop;
      Serial.println(countMag);
    }
  */
  /////////////////////////////////////////////////////////// 
  // here we will try to set the magnitude limit automatic //
  ///////////////////////////////////////////////////////////
  if (magnitude > magnitudelimit_low)
  {
    magnitudelimit = (magnitudelimit +((magnitude - magnitudelimit)/6));  /// moving average filter
  }
  if (magnitudelimit < magnitudelimit_low)
  {
    magnitudelimit = magnitudelimit_low;
  }
  ////////////////////////////////////
  // now we check for the magnitude //
  ////////////////////////////////////

  if(magnitude > magnitudelimit*0.6)
  {                          // just to have some space up 
    realstate = HIGH;
  } 
  else
  {
    realstate = LOW; 
  }
  ///////////////////////////////////////////////////// 
  // here we clean up the state with a noise blanker //
  /////////////////////////////////////////////////////
  if (realstate != realstatebefore)
  {
    laststarttime = millis();
  }
  if ((millis()-laststarttime)> nbtime)
  {
    if (realstate != filteredstate)
    { 
      filteredstate = realstate; 
    }
  }
  ////////////////////////////////////////////////////////////
  // Then we do want to have some durations on high and low //
  ////////////////////////////////////////////////////////////
  if (filteredstate != filteredstatebefore)
  {
    if (filteredstate == HIGH)
    {
      starttimehigh = millis(); lowduration = (millis() - startttimelow); 
    }
    if (filteredstate == LOW) 
    { 
      startttimelow = millis(); highduration = (millis() - starttimehigh);
      if (highduration < (2*hightimesavg) || hightimesavg == 0)
      {
        hightimesavg = (highduration+hightimesavg+hightimesavg)/3;
      }                                                             // now we know avg dit time ( rolling 3 avg)
      if (highduration > (5*hightimesavg) )
        {
          hightimesavg = highduration+hightimesavg; 
        }                                     // if speed decrease fast ..
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // now we will check which kind of baud we have - dit or dah and what kind of pause we do have 1 - 3 or 7 pause //
  // we think that hightimeavg = 1 bit                                                                            //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (filteredstate != filteredstatebefore)
  {
    stop = LOW;
    if (filteredstate == LOW)
    {                                                                                                                           // we did end a HIGH
      if (highduration < (hightimesavg*2) && highduration > (hightimesavg*0.6))
      {                                                // 0.6 filter out false dits
        strcat(code,".");
  // *****************************************************************************************************************************//  
  //Serial.print(".");
      }
      if (highduration > (hightimesavg*2) && highduration < (hightimesavg*6))
      { 
        strcat(code,"-");
  // *****************************************************************************************************************************//  
  //Serial.print("-");
        wpm = ( wpm + (1200/((highduration)/3)))/2;                              // the most precise we can do ;o)
      }
    }
    if (filteredstate == HIGH)
    {                                                                            //// we did end a LOW
      float lacktime = 1;
      if(wpm > 25)lacktime=1.0; ///  when high speeds we have to have a little more pause before new letter or new word 
      if(wpm > 30)lacktime=1.2;
      if(wpm > 35)lacktime=1.5;

      if (lowduration > (hightimesavg*(2*lacktime)) && lowduration < hightimesavg*(5*lacktime))
      {                                                                          // letter space
        docode();
        code[0] = '\0';
                                                               //       Serial.print("/");
      }
      if (lowduration >= hightimesavg*(5*lacktime))
      {                                                        // word space
        docode();
        code[0] = '\0';
        printascii(32);  //32 = Space
                                                               //        Serial.println();
      }
    }
  }
  //////////////////////////////
  // write if no more letters //
  //////////////////////////////
  if ((millis() - startttimelow) > (highduration * 6) && stop == LOW)
  {
    docode(); code[0] = '\0'; stop = HIGH; 
  }
  /////////////////////////////////////
  // we will turn on and off the LED //
  // and the speaker                 //
  /////////////////////////////////////
  if(filteredstate == HIGH)
  {
    digitalWrite(ledPin, HIGH);
   tone(audioOutPin,700);
  }
  else
  {
    digitalWrite(ledPin, LOW);
    noTone(audioOutPin);
  }

  //////////////////////////////////
  // the end of main loop clean up//
  //////////////////////////////////
  updateinfolinelcd();              //Remowe or make comment of this line to decode on all 4 lines in display
  realstatebefore = realstate; 
  lasthighduration = highduration; 
  filteredstatebefore = filteredstate;
}
  ////////////////////////////////
  // translate cw code to ascii //
  ////////////////////////////////
  // **************************************************************************
  // ***  This routine is re-writed by OZ6YM, Palle - http://www.planker.dk ***
  // **************************************************************************
void docode()
{
  for (int i=0; i <= 46; i++)
  { 
    if (strcmp(code,&VARcw0[i][0]) == 0)
    {
      printascii(i+44); 
      break;
    }
  }                                                                                      // Char kendes
  for (int i=0; i <= 7; i++)
  { 
    if (strcmp(code,&VARcw1[i][0]) == 0)
    {
      printascii(VARchr[i]); 
      break;
    }
  }
}    // Char kendes
       
  // **************************************************************************
  // ** END of re-writed code de OZ6YM                                       **
  // **************************************************************************
///////////////////////////////////////////////////////////////////////////////////
// print the ascii code to the lcd one a time so we can generate special letters //
///////////////////////////////////////////////////////////////////////////////////
void printascii(int asciinumber)
{
  int fail = 0;
  if (rows == 4 and colums == 16)fail = -4;                  // to fix the library problem with 4*16 display http://forum.arduino.cc/index.php/topic,14604.0.html
  if (lcdindex > colums-1)
  {
    lcdindex = 0;
    if (rows==4)
    {
      for (int i = 0; i <= colums-1 ; i++)
      {
        lcd.setCursor(i,rows-3);
        lcd.write(line2[i]);
        line2[i]=line1[i];
      }
    }
    for (int i = 0; i <= colums-1 ; i++)
    {
      lcd.setCursor(i+fail,rows-2);
      lcd.write(line1[i]);
      lcd.setCursor(i+fail,rows-1);
      lcd.write(32);
    }
  }
  line1[lcdindex]=asciinumber;
  lcd.setCursor(lcdindex+fail,rows-1);
  lcd.write(asciinumber);
  lcdindex += 1;
}

void updateinfolinelcd()
{
  ///////////////////////////////////////////////////
  // here we update the upper line with the speed. //
  ///////////////////////////////////////////////////
  int place;
  if (rows == 4)
  {
    place = colums/2;
  }
  else
  {
    place = 2;
  }
  // ***************************************************
  // ** here is some modyfied code de OZ6YM           **
  // ***************************************************
  if (wpm<10)
  {
    
    lcd.setCursor((place)-10,0);
    lcd.print(navn);
    lcd.setCursor((place)+4,0);
    lcd.print("0");
    lcd.setCursor((place)+5,0);
    lcd.print(wpm);
    lcd.setCursor((place)+6,0);
    lcd.print(" WPM");
//    lcd.setCursor(0,0);lcd.print(count);
  }
  else
  {
    lcd.setCursor((place)+4,0);
    lcd.print(wpm);
    lcd.setCursor((place)+6,0);
    lcd.print(" WPM");
//    lcd.setCursor(0,0);lcd.print(int(target_freq));
  }
}

