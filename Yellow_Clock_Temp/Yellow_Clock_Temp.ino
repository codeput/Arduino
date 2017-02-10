#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;
#include <dsp7s04b.h>
#include "DHT.h"
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTPIN 2     // what digital pin we're connected to DHT 11
DHT dht(DHTPIN, DHTTYPE);
char buffer[10];
unsigned long ti;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  dsp7s04b.setAddress(EA_DSP7S04_ADDR_DEFAULT);
  dsp7s04b.setBrightness(200);
  RTC.begin();
  dht.begin();
}

void loop () {
  ti = millis(); // Initial time for the Timer of Hour/Time
  while ((millis() - ti) < 10000) {
    DateTime now = RTC.now();
    if (digitalRead(7) == HIGH) {
      sprintf(buffer,  "%02d%02d", now.hour(), now.minute());
    } else sprintf(buffer,  "%02d%02d", now.hour() - 1, now.minute());
    Serial.println(buffer);
    dsp7s04b.println(buffer);
    dsp7s04b.setColon();
    delay(500);
    dsp7s04b.clearColon();
    delay(500);
  }


  int t = dht.readTemperature();
  DateTime now = RTC.now();
  sprintf(buffer,  "%02d  ", t);
  Serial.println(buffer);
  dsp7s04b.println(buffer);
  dsp7s04b.setRaw(2, 0b00000001100011);
  dsp7s04b.setRaw(3, 0b00000000111001);
  delay(5000);
}
