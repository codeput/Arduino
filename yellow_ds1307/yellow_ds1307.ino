#include "Wire.h"
#include "RTClib.h"

RTC_DS1307 RTC;
#include <dsp7s04b.h>

char buffer[10];
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  dsp7s04b.setAddress(EA_DSP7S04_ADDR_DEFAULT);
  dsp7s04b.setBrightness(127);
  RTC.begin();

}

void loop () {

  DateTime now = RTC.now();
  sprintf(buffer,  "%02d%02d", now.hour(), now.minute());
  Serial.println(buffer);
  dsp7s04b.println(buffer);
  dsp7s04b.setColon();
  delay(500);
  dsp7s04b.clearColon();
  delay(500);
}
