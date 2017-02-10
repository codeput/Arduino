#include "Wire.h"
#include "RTClib.h"
RTC_DS1307 RTC;
#include <dsp7s04b.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
unsigned long ti;
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
char buffer[10];
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
