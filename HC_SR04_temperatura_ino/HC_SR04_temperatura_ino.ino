/*
Per HC-SR04
Temperatura variabile
Il circuito:
VVC del sensore connesso a +5V
GND del sensore connesso a GND
TRIG del sensore connesso al pin digitale 12
ECHO del sensore connesso al pin digitale 13
*/

//includo le librerie wire e del sensore di temperatura e pressione BMP085
#include <dht11.h>
// DHT11 sensor pins
const int dht11Pin = 2;
dht11 DHT11;
//dichiaro in due variabili i pin del sensore ad ultrasuoni HC-SR04 per misurare l'altezza della neve
const int TRIG_PIN = 3;
const int ECHO_PIN = 4;

void setup() {
  
  Serial.begin(9600); //inizializzo la comunicazione seriale
  pinMode(TRIG_PIN, OUTPUT); //dichiaro come output il pin trig
  pinMode(ECHO_PIN, INPUT); //dichiaro come input il pin echo
}

void loop() {
  float temperatura = DHT11.temperature -2 ;
  
  float distanza, durata; //dichiaro la variabile neve e la variabile durata
  float cmPerMicrosecondi = 0.0331 + ( 0.000062 * temperatura); //calcolo i cm/ms del suono in base alla temperatura
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2); 
  digitalWrite(TRIG_PIN, HIGH); 
  delayMicroseconds(10); //faccio un impulso di dieci microsecondi sul pin trig
  digitalWrite(TRIG_PIN, LOW);
  durata = pulseIn(ECHO_PIN,HIGH); //mi metto in ascolto sul pin eco e calcolo la durata dell'impulso
  distanza = (durata*cmPerMicrosecondi/2.0); //calcolo la distanza
  if (distanza <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(temperatura);
    Serial.print(" *C  ");
    Serial.print(distanza);
    Serial.println(" cm ");
    Serial.println();
  }
  delay(1000);
  
}
