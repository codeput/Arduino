
/*

mfrc522_id_authenticate
by N0ZYC  4/16/2014
based on example sketches

This sketch demonstrates how to use RFID tags for identification or authentication
MRF-522 reader is compatible with 13.56 MHz RFID tags: mifare1 S50, mifare1 S70 MIFARE Ultralight, mifare Pro, MIFARE DESFire
http://www.nxp.com/documents/data_sheet/MFRC522.pdf

* Pin layout should be as follows:
* Signal     Pin              Pin               Pin
*            Arduino Uno      Arduino Mega      MFRC522 board
* ------------------------------------------------------------
* Reset      9                5                 RST              reset,               can be changed on arduino, defined below
* SPI SS     10               53                SDA              slave select,        can be changed on arduino, defined below

* SPI MOSI   11               51                MOSI             master out slave in, hard-wired, varies between arduino models
* SPI MISO   12               50                MISO             master in slave out, hard-wired, varies between arduino models
* SPI SCK    13               52                SCK              spi clock,           hard-wired, varies between arduino models

BE CAREFUL!  The modules commonly available require 3.3 volts on their VCC
this module does not appear to have a level converter for 5v spi logic, but mine hasn't cooked mine yet

*/





//  include SPI and RFID libraries
#include <SPI.h>
#include <MFRC522.h>  //  library available at https://github.com/miguelbalboa/rfid





//  define changeable pins
#define SS_PIN 10  //  all SPI peripherals share the same predefined pins for miso, mosi, and sck, but each peripheral uses its own Slave Select
#define RST_PIN 9  //  the reset pin is not part of the SPI protocol, and can be shared or changed





//  operational configurations
#define startup_chirp true   //  define true to chirp twice on power-up
#define debugging     false  //  define true to display more information during read attempts





//  you can attach a little piezo beeper to make a chirp in response to reading a valid or invalid token
//  tie (-) on the beeper to ground and (+) to the specified pin:
#define chirp_dpin 8          //  define this to chirp when a token registers
#define chirp_valid_ms   100  //  define this to chirp this many ms when a valid token is presented
#define chirp_invalid_ms  10  //  define this to chirp this many ms when an invalid token is presented
//  after you're done testing, for security purposes, you may want to leave chirp_invalid_ms set to 0





//  Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);





//  create a list of valid RFID tokens
#define user_count 16
#define token_invalid -1  //  invalid token ID or user ID
unsigned long user_tokens [user_count] = {
  0x745CB413,0x745ADFED,0x745F0CEE,0x745DABC2,0x745E56C1,0xA9BA5D53,0xA9BE5843,0xA9BB5773,
//  0xA9C22333,0xA9B995F3,0x3281DE64,0x326733B4,0x3287CF24,0x32886664,0x3282A744,0x328755F4
  0xA9C22333,0xA9B995F4,0x3281DE64,0x326733B4,0x3287CF24,0x32886664,0x3282A744,0x328755F4  //  invalidated token B5 for testing
};

/*
A1 - A5: (my white cards)
Card UID: 13 B4 5D 74  ($745CB413 / 1952232467)   0 = A1
Card UID: ED DF 5B 74  ($745ADFED / 1952112621)   1 = A2
Card UID: EE 0C 5F 74  ($745F0CEE / 1952386286)   2 = A3
Card UID: C2 AB 5E 74  ($745DABC2 / 1952295874)   3 = A4
Card UID: C1 56 5E 74  ($745E56C1 / 1952339649)   4 = A5  secondary test token

B1 - B5: (my blue keyfobs)
Card UID: 53 5D BA A9  ($A9BA5D53 / 2847563091)   5 = B1
Card UID: 43 58 BE A9  ($A9BE5843 / 2847823939)   6 = B2
Card UID: 73 57 BB A9  ($A9BB5773 / 2847627123)   7 = B3
Card UID: 33 23 C2 A9  ($A9C22333 / 2848072499)   8 = B4
Card UID: F3 95 BA A9  ($A9B995F3 / 2847512051)   9 = B5  secondary test token

C1 - C6: (my tan keyfobs)
Card UID: 64 DE 82 32  ($3281DE64 / 847371876)  19 = C1
Card UID: B4 33 67 32  ($326733B4 / 845624244)  11 = C2
Card UID: 24 CF 88 32  ($3287CF24 / 847761188)  12 = C3
Card UID: 64 66 88 32  ($32886664 / 847799908)  13 = C4
Card UID: 44 A7 83 32  ($3282A744 / 847423300)  14 = C5
Card UID: F4 55 87 32  ($328755F4 / 847730164)  15 = C6  primary test token
*/





void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC
  delay(100);
  SPI.begin();    // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  //  init the chirp
  #ifdef chirp_dpin
    pinMode(chirp_dpin,OUTPUT);
    //  make a double chirp at startup if so configured
    if (startup_chirp) {
      digitalWrite(chirp_dpin,HIGH);
      delay(50);
      digitalWrite(chirp_dpin,LOW);
      delay(50);
      digitalWrite(chirp_dpin,HIGH);
      delay(50);
      digitalWrite(chirp_dpin,LOW);
      delay(50);
    }
  #endif
  Serial.println("\n\nReady to begin identifying cards, scan cards now:\n\n");
}





//  return a user id (index) based on provided token id (return token_invalid if not found in user list)
int UserFromToken(unsigned long got_id) {
  for (uint16_t i = 0 ; i < user_count ; i++) {
    if (got_id == user_tokens[i]) {
      return i;
    }
  }
  return token_invalid;
}





//  return the ID of the user whose token has just been presented, or return token_invalid if no new valid token
int UserTokenPresented (boolean got_multi = false, boolean got_debug = false) {
  //  return token_invalid if no new token has been presented
  if (not mfrc522.PICC_IsNewCardPresent()) {
    return token_invalid;
  }
  //  return token_invalid if unable to read presented token
  if (not mfrc522.PICC_ReadCardSerial()) {
    return token_invalid;
  }
  //  a new functional (though not necessarily valid) token has been presented AND read
  // halt the selected card.  prevent another read moments from now (unless requested)
  if (not got_multi) {
    mfrc522.PICC_HaltA();
  }
  //  calculate token id and look up user id.  use only the first four bytes of the id (should be little endian, length valid on any token almost guaranteed unique)
  unsigned long token_id = mfrc522.uid.uidByte[0] + mfrc522.uid.uidByte[1] * 0x100 + mfrc522.uid.uidByte[2] * 0x10000 + mfrc522.uid.uidByte[3] * 0x1000000;
  int user_id = UserFromToken(token_id);
  //  print token information if requested by caller
  if (got_debug) {
    Serial.print("Card UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.print("  ($");
    Serial.print(token_id,HEX);
    Serial.print(" / ");
    Serial.print(token_id,DEC);
    Serial.print(") user_id = ");
    Serial.print(user_id,DEC);
    if (user_id == token_invalid) {
      Serial.print(" (invalid token / no such user)");
    }
    Serial.println();
  }
  //  chirp the beeper if configured to do so
  #ifdef chirp_dpin
    if ((user_id != token_invalid) && (chirp_valid_ms > 0)) {
      //  token is a valild user and we are supposed to chirp
      digitalWrite(chirp_dpin,HIGH);
      delay(chirp_valid_ms);
      digitalWrite(chirp_dpin,LOW);
    } else if ((user_id == token_invalid) && (chirp_invalid_ms > 0)) {
      //  token is NOT a valild user and we are supposed to chirp
      digitalWrite(chirp_dpin,HIGH);
      delay(chirp_invalid_ms);
      digitalWrite(chirp_dpin,LOW);
    }
  #endif
  //  return the user id (index) or token_invalid
  return user_id;
}





//  test loop - acknowledge any valid tokens
void loop() {
  
  int user_id = UserTokenPresented(false,debugging);
  if (user_id != token_invalid ) {
    Serial.print("Valid user ID # "); Serial.print(user_id,DEC); Serial.println(" identified.");
  }

}

