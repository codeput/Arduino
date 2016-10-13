

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <dht11.h>
#define DHTPIN 4 //pin gpio 12 in sensor
dht11 DHT11;
float h = 0;
float t = 0;
char auth[] = "0b6e3b4d57914fda8134cf980ef9b809";

SimpleTimer timer;
void sendUptime()
{
  DHT11.read(DHTPIN);
  t = DHT11.temperature - 2 ;
  Blynk.virtualWrite(10, t);
  h = DHT11.humidity ;
  Blynk.virtualWrite(11, h);
  Serial.println(t);
  Serial.println(h);
  int s = digitalRead(2);
  int tin = 17.00;
  int tout = 20.00;
  int t1 = (t - 2);
  if ((t1 <= tin) && (s == 1)) {
    digitalWrite(2, 1);
  }
  if ((t1 >= tout) && (s == 1)) {
    digitalWrite(2, 0);
  }
}
void setup()
{
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(4, INPUT);
  Blynk.begin(auth, "Telecom-71694417", "qqqqqqqq", "emax.tcp4.me");
  timer.setInterval(1000L, sendUptime);
}
bool isFirstConnect = true;

BLYNK_CONNECTED() {
  if (isFirstConnect) {
    Blynk.syncAll();
    isFirstConnect = false;
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
