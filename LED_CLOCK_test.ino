// Simply change commonCathode to 1 for common cathode 7-seg display
#define commonCathode 1
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <EEPROM.h>
#include <Bounce.h>
#include <pitches.h>
#include <dht11.h>
#include <DS1302.h>

// Init the DS1302
DS1302 rtc(2, 3, 8);
#define DS1302_GND_PIN 4
#define DS1302_VCC_PIN 5

// Shift register pins
const int latchPin = 7;
const int clockPin = 6;
const int dataPin = 13;

// Select digit pins
const int digit1 = 12;
const int digit2 = 11;
const int digit3 = 10;
const int digit4 = 9;

// alarm pin
const int alarmTonePin = 14;
const int alarmControlPin = 16;
const int leftButtonPin = 18;
const int rightButtonPin = 19;

// DHT11 sensor pins
const int dht11Pin = 15;

// EEPROM locations
const int minuteLocation = 0;
const int hourLocation = 1;
const int alarmOnLocation = 2;
const int hoursetLoc = 3;
const int minutesetLoc = 4;
const int timelocation = 5;



// Instantiate a Bounce object with a 20 millisecond debounce time
Bounce alarmControlButton = Bounce(alarmControlPin, 20);
Bounce leftButton = Bounce(leftButtonPin, 20);
Bounce rightButton = Bounce(rightButtonPin, 20);

dht11 DHT11;
int temperature;

int minute;
int hour;

Time t;

/*
  11 digits:
  Each defines which segments should be on/off for that digit: A,B,C,D,E,F,G,P
*/

const byte numbers[12] =
{
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110,  // 9
  B10011100,  // C
  B00000001  // .
};

byte alarmMin;
byte alarmHour;
byte timeHour;
byte timeMin;
boolean alarmOn = false;

boolean setupAlarm = false;
boolean setuptime = false;

int noteIndex = 0;
const int noteCount = 8;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

boolean leftButtonPressed = false;
boolean rightButtonPressed = false;
boolean alarmControlButtonPressed = false;

boolean displayAlarm = true;
boolean showTime = true;
long alarmSwapTime = -1;
long timeSwapTime = -1;

boolean playingAlarm = false;
boolean attivo = false;


void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  pinMode(alarmControlPin, INPUT);
  pinMode(rightButtonPin, INPUT);
  pinMode(leftButtonPin, INPUT);


  digitalWrite(DS1302_GND_PIN, LOW);
  pinMode(DS1302_GND_PIN, OUTPUT);
  digitalWrite(DS1302_VCC_PIN, HIGH);
  pinMode(DS1302_VCC_PIN, OUTPUT);

  rtc.halt(false);
  //uncomment to initialize rtc
  //disable the write protection
  //rtc.writeProtect(false);
  //rtc.setDOW(MONDAY);        // Set Day-of-Week
  // rtc.setTime(18, 49, 0);    // Set the time
  // rtc.setDate(07, 3, 2016);   // Set the date

  seTime();

  resetDigits();

  Wire.begin();

  alarmMin = EEPROM.read(minuteLocation);
  alarmHour = EEPROM.read(hourLocation);
  alarmOn = EEPROM.read(alarmOnLocation);




}



void resetDigits() {
  digitalWrite(digit1, HIGH); // common cathode
  digitalWrite(digit2, HIGH); // common cathode
  digitalWrite(digit3, HIGH); // common cathode
  digitalWrite(digit4, HIGH); // common cathode
}

void sevenSegWrite(byte digit, int digitPosition) {
  byte pin = 2;
  int number =  numbers[digit];
  resetDigits();
  digitalWrite(digitPosition, LOW);
  updateShiftRegister(number);
}

/// MODIFICA 1

void seTime () {
  leftButton.update();
  int leftValue = leftButton.read();

  rightButton.update();
  int rightValue = rightButton.read();

  alarmControlButton.update();
  int alarmControlValue = alarmControlButton.read();

  if (alarmControlValue == HIGH) {
    if (!alarmControlButtonPressed) {
      alarmControlButtonPressed = true;
      if (playingAlarm) {
        EEPROM.write(timelocation, 0);
        return;
      }
      if (!setuptime) {
        setuptime = true;
        Serial.println("setuptime = true");
      }
      else {
        setuptime = false;
        EEPROM.write(timelocation, 1);
        Serial.println("setuptime = false");
      }
    }
  }
  else {
    alarmControlButtonPressed = false;
  }

  if ( leftValue == HIGH) {
    if (!leftButtonPressed) {
      Serial.println("Left button pressed");
      leftButtonPressed = true;
      if (setuptime) {
        timeHour++;
        if (timeHour >= 24) {
          timeHour = 0;
        }
        EEPROM.write(hoursetLoc, timeHour);
      }
    }
  }
  else {
    leftButtonPressed = false;
  }

  if ( rightValue == HIGH) {
    if (!rightButtonPressed) {
      Serial.println("Right button pressed");
      rightButtonPressed = true;
      if (setuptime) {
        timeMin++;
        if (timeMin >= 60) {
          timeMin = 0;
        }
        EEPROM.write(minutesetLoc, timeMin);
      }
    }
  }
  else {
    rightButtonPressed = false;

    timeMin = EEPROM.read(minutesetLoc);
    timeHour = EEPROM.read(hoursetLoc);
    rtc.writeProtect(false);
    rtc.setTime(hoursetLoc, minutesetLoc, 0);
    delay(500);
    rtc.writeProtect(true);
  }
}

//MODIFICA 1 FINE




void loop() {

  t = rtc.getTime(); // Read time from DS1302

  checkAlarmButtons();

  if (!setupAlarm) {
    if (playAlarm(minute, hour) && (attivo == 1)) {
      playingAlarm = true;
      if (playTone(melody[noteIndex], noteDurations[noteIndex])) {
        noteIndex++;
        if (noteIndex >= noteCount) {
          noteIndex = 0;
        }
      }
    }
    else {
      playingAlarm = false;
    }
    if (showTime) {
      displayTime(minute, hour);
    }
    else {
      displayTemperature();
    }
    displayCycle();
  }
  else {
    if (displayAlarm) {
      showAlarm();
    }
    else {
      hideAlarm();
    }
    alarmCycle();
  }
}

void alarmCycle() {
  if (millis() - alarmSwapTime >= 500) {
    alarmSwapTime = millis();
    displayAlarm = !displayAlarm;
    Serial.print("Alarm cycle:");
    Serial.println(alarmSwapTime);
  }
}

void displayCycle() {
  boolean swap;
  if ((!showTime && millis() - timeSwapTime >= 2000) || (showTime && millis() - timeSwapTime >= 10000)) {
    timeSwapTime = millis();
    showTime = !showTime;
    if (!showTime) {
      int chk = DHT11.read(dht11Pin);
      temperature = (int)DHT11.temperature;
      Serial.println(temperature);
    } else {
      hour = t.hour;
      minute = t.min;
    }
  }
}

void showAlarm() {
  int alarmMin = EEPROM.read(0);
  int alarmHour = EEPROM.read(1);
  displayTime(alarmMin, alarmHour);
}

void hideAlarm() {
  resetDigits();
}

void checkAlarmButtons() {
  leftButton.update();
  int leftValue = leftButton.read();

  rightButton.update();
  int rightValue = rightButton.read();

  alarmControlButton.update();
  int alarmControlValue = alarmControlButton.read();

  if (alarmControlValue == HIGH) {
    if (!alarmControlButtonPressed) {
      alarmControlButtonPressed = true;
      if (playingAlarm) {
        EEPROM.write(alarmOnLocation, 0);
        return;
      }
      if (!setupAlarm) {
        setupAlarm = true;
        Serial.println("setupAlarm = true");
      }
      else {
        setupAlarm = false;
        EEPROM.write(alarmOnLocation, 1);
        Serial.println("setupAlarm = false");
      }
    }
  }
  else {
    alarmControlButtonPressed = false;
  }

  if ( leftValue == HIGH) {
    if (!leftButtonPressed) {
      Serial.println("Left button pressed");
      leftButtonPressed = true;
      attivo = !attivo;
      Serial.println(attivo);
      if (setupAlarm) {
        alarmHour++;
        if (alarmHour >= 24) {
          alarmHour = 0;
        }
        EEPROM.write(hourLocation, alarmHour);
      }
    }
  }
  else {
    leftButtonPressed = false;
  }

  if ( rightValue == HIGH) {
    if (!rightButtonPressed) {
      Serial.println("Right button pressed");
      rightButtonPressed = true;
      if (setupAlarm) {
        alarmMin++;
        if (alarmMin >= 60) {
          alarmMin = 0;
        }
        EEPROM.write(minuteLocation, alarmMin);
      }
    }
  }
  else {
    rightButtonPressed = false;
  }
}

void displayTime(int minute, int hour) {
  int minLSB = minute % 10;
  //sevenSegWrite(minLSB, digit1);
  //updateShiftRegister(0);
  if (attivo == 1) {
    sevenSegWrite(minLSB, digit1);
    sevenSegWrite(11, digit1);
    updateShiftRegister(0);
  } else
    sevenSegWrite(minLSB, digit1);
  updateShiftRegister(0);


  int minMSB = minute / 10;
  sevenSegWrite(minMSB, digit2);
  updateShiftRegister(0);

  int hourLSB = hour % 10;
  sevenSegWrite(hourLSB, digit3);
  // sevenSegWrite(, digit3);
  updateShiftRegister(0);

  int hourMSB = hour / 10;
  sevenSegWrite(hourMSB, digit4);
  updateShiftRegister(0);

  sevenSegWrite(11, digit3);
  updateShiftRegister(0);
}

void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

long noteStartTime = -1;

boolean playAlarm(int minute, int  hour) {
  byte _minute = EEPROM.read(minuteLocation);
  byte _hour = EEPROM.read(hourLocation);
  return EEPROM.read(alarmOnLocation) == 1 && _minute == minute && _hour == hour;
}

boolean playTone(int note, int noteDuration) {
  // to calculate the note duration, take one second
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int duration = 1000 / noteDuration;
  tone(alarmTonePin, note, duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if (noteStartTime == -1) {
    noteStartTime = millis();
    return false;
  }
  else if (noteStartTime != -1 && millis() - noteStartTime >= duration * 1.3) {
    noTone(alarmTonePin);
    noteStartTime = -1;
  }
  else {
    return false;
  }
  return true;

}

void displayTemperature() {
  sevenSegWrite(10, digit1);
  updateShiftRegister(0);

  sevenSegWrite(temperature % 10, digit2);
  updateShiftRegister(0);

  sevenSegWrite(temperature / 10, digit3);
  updateShiftRegister(0);

  digitalWrite(digit4, HIGH); // common cathode
}






















