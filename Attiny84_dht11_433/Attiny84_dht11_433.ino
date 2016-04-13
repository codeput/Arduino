//will work on Attiny85/45

//RF433=D0 pin 5
//DHT11=D4 pin 3

// libraries
#include <dht11.h> //From Rob Tillaart
#include <Manchester.h>
dht11 DHT11;
#define DHT11PIN 0
#define TX_PIN 1  //pin where your transmitter is connected
int latchpin = 5;// connect to pin 12 on the '595
int clockpin = 7; // connect to pin 11 on the '595
int datapin = 6; // connect to pin 14 on the '595
// the array contains the binary value to make digits 0-9
// for the number '11'  use code '31' for CC and '224' for CA
int segment[10] = {63, 57, 91, 79, 102, 109, 125, 7, 127, 111 }; // for common cathode
//int segment[10] = {192,249,164,176,153,146,130,248,128,144 }; // for common anode
//variables
float h = 0;
float t = 0;
float temp = 0;
int transmit_t = 0;
int transmit_h = 0;
int transmit_data = 0;

void setup()
{
  pinMode(1, INPUT);
  man.setupTransmit(TX_PIN, MAN_1200);
  pinMode(latchpin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  pinMode(datapin, OUTPUT);
}

void loop() {
  digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, MSBFIRST, 0); // clears the display
  digitalWrite(latchpin, HIGH);
  int chk = DHT11.read(DHT11PIN);
  h = DHT11.humidity;
  temp = DHT11.temperature;
  t = temp - 2;
  // I know, I am using 3 integer variables here
  // where I could be using 1
  // but that is just so it is easier to follow

  transmit_h = 100 * (int) h;
  transmit_t = (int) t;
  transmit_data = transmit_h + transmit_t;
  man.transmit(transmit_data);
  if (t >= 25) {
    digitalWrite(latchpin, LOW);
    shiftOut(datapin, clockpin, MSBFIRST, segment[2] + 128);
    digitalWrite(latchpin, HIGH);
  } else
    digitalWrite(latchpin, LOW);
  shiftOut(datapin, clockpin, MSBFIRST, segment[1] + 128);
  digitalWrite(latchpin, HIGH);
  delay(500);
}
