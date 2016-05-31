/*
 Test_lettura_sensore_LM35.pde
 Il programma misura la temperatura tramite il sensore LM35
 e mostra il valore sul display LCD
 
 Vengono utilizzati i seguenti pin
 Pin +5V         -> Alimentazione
 Pin GND         -> Alimentazione
 Pin Digital 4   -> Bus D4 dati LCD
 Pin Digital 5   -> Bus D5 dati LCD
 Pin Digital 6   -> Bus D6 dati LCD
 Pin Digital 7   -> Bus D7 dati LCD
 Pin Digital 8   -> terminale RS display LCD
 Pin Digital 9   -> terminale EN display LCD
 Pin Analogico 1 -> lettura LM35
 
 La scala dell'LM 35 è T = K * V
 dove:
 K = 100 [°C/V] coeff.angolare della scala
 V = tensione generata dall'LM35 (10mV/°C)
 T = temperatura in °C
 
 Creato il 15/12/2011
 da Adriano Gandolfo <http://www.adrirobot.it>
 This example code is in the public domain.
 
/* Include la libreria per la gestione del LCD */

#include <LiquidCrystal.h>

/* Corrispondenza pin LCD <-> pin digitali di Arduino */

#define RS  8
#define EN  9
#define D7  7  
#define D6  6
#define D5  5
#define D4  4

/* Numero del pin analogico sul quale è collegato l'LM35  */
#define LM35_pin 1

/* Definizioni globali */
float vref = 1.1;	                     // Vref dell'ADC (quell'interno è di 1,1V)
LiquidCrystal lcd( RS, EN, D4, D5, D6, D7 ); // 'lcd' è una variabile di tipo LiquidCrystal */


/* Impostazione dell'hardware */

void setup() 
{
  analogReference( INTERNAL );  // per l'ADC usiamo il Vref interno da 1,1V (migliore precisione)
  analogRead( LM35_pin );       // Prima lettura "a vuoto" (serve per l'assestamento dell'ADC)
  lcd.begin( 2, 16 );           // Impostazione per l' LCD (2x16)
}

void loop()
{
  float temp = readTemp();  // legge il valore della temperatura e la memorizza nella var. temp.
  sendTempToLCD( temp );    // invia il valore al LCD
}

/* Legge la temperatura */
float readTemp()
{
  float temp = 0.0;       // valore convertito in temperatura (°C)
  int val = 0;            // valore quantizzato dall'ADC [0..1023]
  int nread = 5;          // numero di letture (da 5 a 8)
  float somma = 0.0;      // somma delle letture
  for (int i=0; i<nread; i++)
  {
    val = analogRead( LM35_pin );              // legge il dato della tensione sul pin 'LM35_pin' 
    temp = ( 100.0 *  vref * val ) / 1024.0;   // lo converte in °C
    somma += temp;                             // aggiunge alla somma delle temperature lette   
  }   
  return ( somma / nread );                     // ne calvola il valore medio 
}

/* Invia la temperatura su un LCD (modo 4bit) */
void sendTempToLCD( float temp )
{
  lcd.clear();               // Pulisce lo schermo 
  lcd.setCursor( 0, 0 );     // Va in posizione di home: colonna 1, riga 1
  lcd.print( "Temperatura di: ");
  lcd.setCursor( 0, 1 );     // Va in posizione di home: colonna 1, riga 2
  lcd.print( temp );         // Stampa solo la parte intera della temp.
  lcd.print( ' ' );          // Stampa uno spazio
  lcd.print( (char) 223 );   // Stampa '°'
  lcd.print( 'C' );
  delay( 250 );
}




