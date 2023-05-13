// Common code

// Common pin usage
// Note there are additional pins unique to transmitter or receiver
//

// nRF24L01 uses SPI which is fixed on pins 11, 12, and 13.
// It also requires two other signals
// (CE = Chip Enable, CSN = Chip Select Not)
// Which can be any pins:

const int CEPIN = 9;
const int CSNPIN = 10;

// In summary,
// nRF 24L01 pin    Arduino pin   name
//          1                     GND
//          2                     3.3V
//          3             9       CE
//          4             10      CSN
//          5             13      SCLK
//          6             11      MOSI/COPI
//          7             12      MISO/CIPO

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(CEPIN, CSNPIN);  // CE, CSN

const byte addr = 0x76;             
const int RF24_CHANNEL_NUMBER = 0; 

// Do not make changes here
const byte xmtrAddress[] = { addr, addr, 0xC7, 0xE6, 0xCC };
const byte rcvrAddress[] = { addr, addr, 0xC7, 0xE6, 0x66 };

const int RF24_POWER_LEVEL = RF24_PA_LOW;

// global variables
uint8_t pipeNum;
unsigned int totalTransmitFailures = 0;

struct DataStruct {
  uint8_t servoBits;
  uint8_t neoPixelBits;
  uint8_t playbackControlBits;
};
DataStruct data;

char msg[10];

void setupRF24Common() {

  // RF24 setup
  if (!radio.begin()) {
    Serial.println(F("radio  initialization failed"));
    while (1)
      ;
  } else {
    Serial.println(F("radio successfully initialized"));
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF24_CHANNEL_NUMBER);
  radio.setPALevel(RF24_POWER_LEVEL);
}

void rf24RecvData() {
    // If there is data, read it,
  // and do the needfull
  // Become a receiver
  radio.startListening();
  if (radio.available(&pipeNum)) {
    radio.read(&msg, sizeof(msg));
    Serial.print(F("message received Data = "));
    Serial.println(msg);
  }
  
}

void rf24SendData() {

  // The write() function will block
  // until the message is successfully acknowledged by the receiver
  // or the timeout/retransmit maxima are reached.
  int retval = radio.write(&msg, sizeof(msg));
//
//  Serial.print(F("Servo bits = "));
//  Serial.print(data.servoBits, BIN);
//  Serial.print(F(" NeoPixel bits = "));
//  Serial.print(data.neoPixelBits, BIN);
//  Serial.print(F(" Playback control bits = "));
//  Serial.print(data.playbackControlBits, BIN);
//
//  Serial.print(F(" ... "));
  if (retval) {
    Serial.println(F("success"));
  } else {
    totalTransmitFailures++;
    Serial.print(F("failure, total failures = "));
    Serial.println(totalTransmitFailures);
  }
}


// Receiver Code

void setup() {
  Serial.begin(9600);

  setupRF24receiver();

}
void setupRF24receiver() {
  setupRF24Common();
  // Set us as a receiver
  radio.openWritingPipe(rcvrAddress);
  radio.openReadingPipe(1, xmtrAddress);
  // radio.printPrettyDetails();
  Serial.println(F("I am a receiver"));
}

void loop() {
  rf24RecvData();
  


//    Serial.print(F(" NeoPixel bits = "));
//    Serial.print(data.neoPixelBits, BIN);
//    Serial.print(F(" Playback control bits = "));
//    Serial.println(data.playbackControlBits, BIN);
 
   
}  // end of loop()
// end of receiver code
