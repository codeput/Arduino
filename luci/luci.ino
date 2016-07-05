//Sketch per il controllo di un'elettrovalvola
//per irrigazione ......Emanuele Principi 2016

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

//Inserimento Orario Partenza
int ora_c_a = 19;     //Inserire ora di accensione in formato 24H
int minuti_c_a = 45; //Inserire minuto di accensione
int ora_p_a = 20;     //Inserire ora di accensione in formato 24H
int minuti_p_a = 00; //Inserire minuto di accensione
int ora_c_s = 23;     //Inserire ora di accensione in formato 24H
int minuti_c_s = 30; //Inserire minuto di accensione
int ora_p_s = 06;     //Inserire ora di accensione in formato 24H
int minuti_p_s = 30; //Inserire minuto di accensione
//Fine Inserimento

#define relay1 3
#define relay2 4
boolean solenoide = true;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  // Activate RTC module
  rtc.begin();

  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
}

void loop() {


  luce_1();
  luce_2();

}

void luce_1 () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  if ((h == ora_c_a) && (m == minuti_c_a)) {
    digitalWrite(relay1, HIGH);
  }

  if ((h == ora_c_s) && (m == minuti_c_s)) {
    digitalWrite(relay1, LOW);
  }
}
void luce_2 () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  if ((h == ora_p_a) && (m == minuti_p_a)) {
    digitalWrite(relay2, HIGH);
  }

  if ((h == ora_p_s) && (m == minuti_p_s)) {
    digitalWrite(relay2, LOW);
  }
}


