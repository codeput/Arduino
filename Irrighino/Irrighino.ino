//Sketch per il controllo di un'elettrovalvola
//per irrigazione ......Emanuele Principi 2016

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
//Inizializzazione variabili e costanti
int ora = 8;     //Inserire ora di accensione in formato 24H
int minuti = 34; //Inserire minuto di accensione
//
int nora = 0 ;
int nminuti = 0;
int durata = 0;
int somma = 0;
int ContatorePulsantePremuto = 0;
int StatoPulsante = 0;
int StatoPulsantePrecedente = 0;
int umidita = 350;
int h = 0;
int m = 0;


#define BUTTON 8
#define LED1 9
#define LED2 10
#define LED3 11
#define LED4 12
#define relay1 2



void setup() {
  //  Serial.begin(115200);
  Wire.begin();
  // Activate RTC module
  rtc.begin();
  pinMode(relay1, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);                            // imposta il pin digitale come output
}

void loop() {
  somma = minuti + durata;

  //Calcolo minuti
  if (somma >= 60) {
    nora = (ora + 1);
    nminuti = (somma - 60);
  } else nminuti = somma;
  nora = ora;
  //Fine Calcolo minuti

  StatoPulsante = digitalRead(BUTTON);              // legge il valore dell'input e lo conserva

  if (StatoPulsante != StatoPulsantePrecedente) {   // compara lo stato del pulsante attuale con il precedente
    if (StatoPulsante == HIGH) {                    // se lo stato è cambiato incrementa il contatore
      // se lo stato corrente è alto, il pulsante è passato da off a on
      ContatorePulsantePremuto++;

      switch (ContatorePulsantePremuto) {
        case 1:  // controlla se il pulsante è stato premuto una volta
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
          verifico ();
          durata = 0;
          break;
      }
    }
  }

  // salva lo stato corrente nella variabile che indica lo stato precedente per il loop successivo
  StatoPulsantePrecedente = StatoPulsante;

  // controlla se il pulsante è stato premuto quattro volte se vero indica che è finito il ciclo
  // il led lampeggia 2 volte per 50 millisecondi
  // vengono inizializzate nuovamente le variabili
  // si riavvia il ciclo

  if (ContatorePulsantePremuto > 5) {
    digitalWrite(LED4, LOW);                                 // spegne il LED4
    for (int x = 0; x < 5; x++) {                            // ciclo di accensione e spegnimento led
      for (int n = 9; n < 13; n++) {                        // ciclo sui diodi da accendere e spegnere
        digitalWrite(n, HIGH);                               // accende il LED
        delay(50);                                           // aspetta 50 millisecondi
        digitalWrite(n, LOW);                                // spegne il LED
        delay(50);                                           // aspetta 50 millisecondi
        digitalWrite(relay1, LOW);
        durata = 0;
      }
    }
    // inizializzazione delle variabili
    ContatorePulsantePremuto = 0;
    StatoPulsante = 0;
    StatoPulsantePrecedente = 0;
  }
  inizia();
  tempo();
}

void inizia () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  Serial.println(h);
  Serial.println(m);
  if ((h == ora) && (m == minuti)) {
    digitalWrite(relay1, HIGH);
    verifico ();
  }
}
void tempo () {
  DateTime now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  if ((h == nora) && (m == nminuti)) {
    digitalWrite(relay1, LOW);
  }
}
void verifico () {
  int sensorValue = analogRead(A0);
  if (sensorValue >= umidita) {
    digitalWrite(relay1, HIGH);
  } else digitalWrite(relay1, LOW);
}

