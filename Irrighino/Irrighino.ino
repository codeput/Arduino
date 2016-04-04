//Sketch per il controllo di un'elettrovalvola
//per irrigazione ......Emanuele Principi 2016

#include <DS1302.h>

//Inizializzazione variabili e costanti
int ora = 17; //Inserire ora di accensione in formato 24H
int minuti = 52; //Inserire minuto di accensione
int durata = 0; //Inserire durata accensione
//
int nora = 0 ;
int nminuti = 0;
#define BUTTON 8                                    // pin di input a cui è collegato il pulsante  
#define LED1 9                                     // LED collegato al pin digitale 13  
#define LED2 10                                     // LED collegato al pin digitale 12  
#define LED3 11                                     // LED collegato al pin digitale 11  
#define LED4 12   
int somma = minuti + durata;
#define relay1 2
//const int ledPin =  13;
//int ledState = LOW;
//unsigned long previousMillis = 0;
//const long interval = 1000;
int ContatorePulsantePremuto = 0;                   // conta il numero di volte che il pulsante è premuto buttonPushCounter   
int StatoPulsante = 0;                              // stato corrente del pulsante  
int StatoPulsantePrecedente = 0;                    // stato precedente del pulsante 
//Fine Inizializzazione variaibie e costanti

//Creazione oggetti RTC
DS1302 rtc(5, 6, 7);
#define DS1302_GND_PIN 3
#define DS1302_VCC_PIN 4
Time t;
//Fine Creazione oggetti RTC

void setup() {

 // Serial.begin(115200);

  // Activate RTC module
  digitalWrite(DS1302_GND_PIN, LOW);
  pinMode(DS1302_GND_PIN, OUTPUT);
  digitalWrite(DS1302_VCC_PIN, HIGH);
  pinMode(DS1302_VCC_PIN, OUTPUT);
  rtc.halt(false);

  pinMode(relay1, OUTPUT);
 // pinMode(ledPin, OUTPUT);
  pinMode(BUTTON, INPUT);                           // imposta il pin digitale come input  
  pinMode(LED1, OUTPUT);                            // imposta il pin digitale come output  
  pinMode(LED2, OUTPUT);                            // imposta il pin digitale come output  
  pinMode(LED3, OUTPUT);                            // imposta il pin digitale come output  
  pinMode(LED4, OUTPUT);                            // imposta il pin digitale come output  


  // tmElements_t tm;
  // tm.Hour = 22;
  // tm.Minute = 39;
  // tm.Second = 00;
  // tm.Day = 17;
  // tm.Month = 02;
  // tm.Year = 2016 -1970;
  //tmElements_t.Year is the offset from 1970
  //  RTC.write(tm);
  //delay ( 2000 );
  // Setup time library

}

void loop() {
  t = rtc.getTime();
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
        Serial.println("on");                                // stampa sulla console "on"  
        Serial.print("numero di volte tasto premuto:  ");    // stampa sulla console "numero di volte tasto premuto:"  
        Serial.println(ContatorePulsantePremuto, DEC);       // stampa il numero di volte che il pulsante è stato premuto  
        digitalWrite(LED1, HIGH);                            // accende il LED1    
        durata = 10;
        Serial.println("off");                               // stampa sulla console "off"  
        break;  
      case 2:  // controlla se il pulsante è stato premuto due volte  
        Serial.println("on");                                // stampa sulla console "on"  
        Serial.print("numero di volte tasto premuto:  ");    // stampa sulla console "numero di volte tasto premuto:"  
        Serial.println(ContatorePulsantePremuto, DEC);       // stampa il numero di volte che il pulsante è stato premuto  
        digitalWrite(LED1, LOW);                             // spegne il LED1   
        digitalWrite(LED2, HIGH);                            // accende il LED2    
        durata = 20;
        Serial.println("off");                               // stampa sulla console "off"  
        break;  
      case 3:  // controlla se il pulsante è stato premuto tre volte  
        Serial.println("on");                                // stampa sulla console "on"  
        Serial.print("numero di volte tasto premuto:  ");    // stampa sulla console "numero di volte tasto premuto:"  
        Serial.println(ContatorePulsantePremuto, DEC);       // stampa il numero di volte che il pulsante è stato premuto  
        digitalWrite(LED2, LOW);                             // spegne il LED2    
        digitalWrite(LED3, HIGH);                            // accende il LED3
        durata = 30;   
        Serial.println("off");                               // stampa sulla console "off"  
        break;  
      case 4:  // controlla se il pulsante è stato premuto quattro volte  
        Serial.println("on");                                // stampa sulla console "on"  
        Serial.print("numero di volte tasto premuto:  ");    // stampa sulla console "numero di volte tasto premuto:"  
        Serial.println(ContatorePulsantePremuto, DEC);       // stampa il numero di volte che il pulsante è stato premuto  
        digitalWrite(LED3, LOW);                             // accende il LED3    
        digitalWrite(LED4, HIGH);                            // accende il LED4
        durata = 40;    
        Serial.println("off");                               // stampa sulla console "off"  
        break;  
        case 5:  // controlla se il pulsante è stato premuto cinque volte  
        Serial.println("on");                                // stampa sulla console "on"  
        Serial.print("numero di volte tasto premuto:  ");    // stampa sulla console "numero di volte tasto premuto:"  
        Serial.println(ContatorePulsantePremuto, DEC);       // stampa il numero di volte che il pulsante è stato premuto  
        digitalWrite(LED1, HIGH);                             // accende il LED3    
        digitalWrite(LED2, HIGH);                            // accende il LED4
        digitalWrite(LED3, HIGH);
        digitalWrite(LED4, HIGH);
        digitalWrite(relay1, HIGH);
        durata = 0;   
        Serial.println("off");                               // stampa sulla console "off"  
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
    Serial.println("fine ciclo");                            // stampa sulla console "fine ciclo"  
    digitalWrite(LED4, LOW);                                 // spegne il LED4     
    for (int x=0; x<5; x++) {                                // ciclo di accensione e spegnimento led  
      for (int n=9; n<13; n++) {                            // ciclo sui diodi da accendere e spegnere  
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
    Serial.println("mi riavvio");                            // stampa sulla console "mi riavvio"  
  }  
  inizia();
  tempo();
  Serial.println(durata);
}

void inizia () {
  if ((t.hour == ora) && (t.min == minuti)) {
    digitalWrite(relay1, HIGH);
  }
}
void tempo () {
  if ((t.hour == nora) && (t.min == nminuti)) {
    digitalWrite(relay1, LOW);
  }
}

