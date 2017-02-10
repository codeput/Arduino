#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;
#include <dsp7s04b.h>
#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to DHT 11
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
boolean tasto = true;
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
char buffer[10];
unsigned long ti;
int hora, minuto, secondo , giorno , mese , anno;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  dsp7s04b.setAddress(EA_DSP7S04_ADDR_DEFAULT);
  dsp7s04b.setBrightness(40);
  RTC.begin();
  dht.begin();
}

void setting () {
  DateTime now = RTC.now();
  hora = now.hour();
  minuto = now.minute();
  secondo = now.second();
  giorno = now.day();
  mese = now.month();
  anno = now.year();
  while (tasto == false) {
    if (digitalRead(8) == LOW) {
      hora = hora++;
      delay(800);
    }
    if (hora > 23) {
      ora = 0;
    }
    if (digitalRead(9) == LOW) {
      miuto = minuto++;
      delay(800);
    }
    if (minuto > 59) {
      ora = 0;
    }
    RTC.adjust(DateTime(anno, mese, giorno, hora, minuto, secondo)); // Scrivve il nuovo orario nell' RTC
    sprintf(buffer,  "%02d%02d", hora, minuto);
    dsp7s04b.println(buffer);
    if (digitalRead(7) == HIGH) {
      tasto = !tasto;
    }
  }
}

void displaytemp () {
  ti = millis(); // Initial time for the Timer of Hour/Time
  while ((millis() - ti) < 5000) {
    int t = dht.readTemperature();
    DateTime now = RTC.now();
    sprintf(buffer,  "%02d%0d", t);
    Serial.println(buffer);
    dsp7s04b.println(buffer);
    dsp7s04b.setRaw(2, 0b00000001100011); // Genera carattere "°"
    dsp7s04b.setRaw(3, 0b00000000111001); // Genera carattere "C"
    delay(5000);
  }
}

void loop () {
  displaytime ();
  displaytemp ();
}

void displaytime () {
  ti = millis(); // Initial time for the Timer of Hour/Time
  while ((millis() - ti) < 10000) {
    if (digitalRead(7) == LOW) {
      tasto = false;
      setting ();
    }
    DateTime now = RTC.now();
    sprintf(buffer,  "%02d%02d", now.hour(), now.minute());
    Serial.println(buffer);
    dsp7s04b.println(buffer);
    dsp7s04b.setColon();
    delay(500);
    dsp7s04b.clearColon();
    delay(500);
  }
}

