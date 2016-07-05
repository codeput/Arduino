//Sketch per il controllo di un'elettrovalvola
//per irrigazione ......Emanuele Principi 2016

#include <Wire.h>
#include "RTClib.h"
#include <RCSwitch.h>
RTC_DS1307 rtc;
RCSwitch mySwitch = RCSwitch();
//Inserimento Orario Partenza
int ora = 23;     //Inserire ora di accensione in formato 24H
int minuti = 30; //Inserire minuto di accensione
//Fine Inserimento
//Definizione di livello umidità
int umidita = 300;
//Fine Definizione
int nora = 0 ;
int nminuti = 0;
int durata = 0;
int somma = 0;
int ContatorePulsantePremuto = 0;
int StatoPulsante = 0;
int StatoPulsantePrecedente = 0;
int h = 0;
int m = 0;
#define BUTTON 8
#define LED1 9
#define LED2 10
#define LED3 11
#define LED4 12
#define LED5 13
#define relay1 3
#define relay2 4
boolean solenoide = true;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  // Activate RTC module
  rtc.begin();
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
}

void loop() {
  somma = minuti + durata;
  //Conversione minuti in ore
  if (somma >= 60) {
    nora = (ora + 1);
    nminuti = (somma - 60);
  } else nminuti = somma;
  nora = ora;
  if (nora >= 24) {
    nora = nora - 24;
  }
  //Fine Conversione

  StatoPulsante = digitalRead(BUTTON);              // legge il valore dell'input e lo conserva
  if (StatoPulsante != StatoPulsantePrecedente) {   // compara lo stato del pulsante attuale con il precedente
    if (StatoPulsante == HIGH) {                    // se lo stato è cambiato incrementa il contatore
      // se lo stato corrente è alto, il pulsante è passato da off a on
      ContatorePulsantePremuto++;
      switch (ContatorePulsantePremuto) {
        case 1:  // controlla se il pulsante è stato premuto una volta
          digitalWrite(LED2, LOW);
          digitalWrite(LED1, HIGH);                            // accende il LED1
          durata = 10;
          break;
        case 2:  // controlla se il pulsante è stato premuto due volte
          digitalWrite(LED1, LOW);                             // spegne il LED1
          digitalWrite(LED2, HIGH);                            // accende il LED2
          durata = 20;
          break;
        case 3:  // controlla se il pulsante è stato premuto tre volte
          digitalWrite(LED2, LOW);                             // spegne il LED2
          digitalWrite(LED3, HIGH);                            // accende il LED3
          durata = 30;
          break;
        case 4:  // controlla se il pulsante è stato premuto quattro volte
          digitalWrite(LED3, LOW);                             // accende il LED3
          digitalWrite(LED4, HIGH);                            // accende il LED4
          durata = 40;
          break;
        case 5:  // controlla se il pulsante è stato premuto cinque volte
          digitalWrite(LED1, HIGH);                             // accende il LED3
          digitalWrite(LED2, HIGH);                            // accende il LED4
          digitalWrite(LED3, HIGH);
          digitalWrite(LED4, HIGH);
          digitalWrite(relay1, HIGH);
          digitalWrite(relay2, LOW);
          delay(1000);
          digitalWrite(relay1, LOW);
          digitalWrite(relay2, LOW);
          durata = 0;
          break;
      }
    }
  }
  // salva lo stato corrente nella variabile che indica lo stato precedente per il loop successivo
  StatoPulsantePrecedente = StatoPulsante;

  if (ContatorePulsantePremuto > 5) {
    digitalWrite(LED4, LOW);
    delay(200);
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
    delay(1000);
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);

    for (int x = 0; x < 5; x++) {                            // ciclo di accensione e spegnimento led
      for (int n = 9; n < 13; n++) {                        // ciclo sui diodi da accendere e spegnere
        digitalWrite(n, HIGH);                               // accende il LED
        delay(50);                                           // aspetta 50 millisecondi
        digitalWrite(n, LOW);                                // spegne il LED
        delay(50);
        durata = 0;
      }
    }
    // inizializzazione delle variabili
    ContatorePulsantePremuto = 0;
    StatoPulsante = 0;
    StatoPulsantePrecedente = 0;
  }


  if (mySwitch.available()) {
    float value = mySwitch.getReceivedValue();
    if (value == 3342528) {
      digitalWrite(relay1, HIGH);
      digitalWrite(relay2, LOW);
      delay(1000);
      digitalWrite(relay1, LOW);
      digitalWrite(relay2, LOW);
    }

    if (mySwitch.available()) {
      float value = mySwitch.getReceivedValue();
      if (value == 3342336) {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, HIGH);
        delay(1000);
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
      }
      mySwitch.resetAvailable();
    }
  }
  inizia();
  tempo();
  if (solenoide == false) {
    digitalWrite(LED5, HIGH);
  } else digitalWrite(LED5, LOW);
}

void inizia () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  int sensorValue = analogRead(A0);
  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.println("");
  Serial.println(sensorValue);
  Serial.println("");
  if ((durata > 1) && (sensorValue >= umidita) && (h == ora) && (m == minuti) && (solenoide == true)) {
    //if ((h == ora) && (m == minuti)) {
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
    delay(1200);
    digitalWrite(relay1, LOW);;
    digitalWrite(LED4, HIGH);
    solenoide = !solenoide ;
  }
}
void tempo () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  if ((h == nora) && (m == nminuti) && (solenoide == false)) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
    delay(1200);
    digitalWrite(relay2, LOW);
    digitalWrite(LED4, LOW);
    solenoide = true ;
  }
}

