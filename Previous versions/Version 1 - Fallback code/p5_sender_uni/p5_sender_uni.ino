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

//#include <printf.h>  // for debugging

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

char msg[10] = "0";

void setupRF24Common() {

  // RF24 setup
  if (!radio.begin()) {
    //    Serial.println(F("radio  initialization failed"));
    while (1)
      ;
  } else {
    //    Serial.println(F("radio successfully initialized"));
  }

  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(RF24_CHANNEL_NUMBER);
  radio.setPALevel(RF24_POWER_LEVEL);
}
int count = 0;
void rf24SendData() {

  // The write() function will block
  // until the message is successfully acknowledged by the receiver
  // or the timeout/retransmit maxima are reached.

  int retval = radio.write(&msg, sizeof(msg));
  //  if(count%2==0){
  //    strcpy(msg, "ball");
  //  }else{
  //    strcpy(msg, "apple");
  //  }
  //  count++;


  //  Serial.print(F("Servo bits = "));
  //  Serial.print(data.servoBits, BIN);
  //  Serial.print(F(" NeoPixel bits = "));
  //  Serial.print(data.neoPixelBits, BIN);
  //  Serial.print(F(" Playback control bits = "));
  //  Serial.print(data.playbackControlBits, BIN);

  //  Serial.print(F(" ... "));
  if (retval) {
    //    Serial.println(F("success"));
  } else {
    totalTransmitFailures++;
    //    Serial.print(F("failure, total failures = "));
    //    Serial.println(totalTransmitFailures);
  }
}

// Transmitter code

void setup() {
  Serial.begin(9600);
  setupRF24transmitter();

  // start the handshake
  while (Serial.available() <= 0) {
    Serial.println("0,0"); // send a starting message
    delay(300);            // wait 1/3 second
  }
}

void setupRF24transmitter() {

  setupRF24Common();

  // Set us as a transmitter
  radio.openWritingPipe(xmtrAddress);
  radio.openReadingPipe(1, rcvrAddress);

  // radio.printPrettyDetails();
  //  Serial.println(F("I am a transmitter"));
}

int p5LeftRecv = 0;
int p5RightRecv = 0;
int p5LeftSend = 0;
int p5RightSend = 0;

char wirelessArduinoRight[2];

void loop() {

  clearData();

  while (Serial.available()) {

    p5LeftRecv = Serial.parseInt();
    p5RightRecv = Serial.parseInt();
    strcpy("msg", "");
    if (Serial.read() == '\n') {

      if (p5LeftRecv == 1) { //auto
        strcpy(msg, "1,0");
        p5LeftSend = 1;
      }
      else if (p5LeftRecv == 0) { //stop
        strcpy(msg, "0,0");
        p5LeftSend = 0;
      }
      else if (p5LeftRecv == 2) { //manual
        strcpy(msg, "2");
        p5LeftSend = 2;
        if (p5RightRecv == 0) {
          strcat(msg, ",0");
        }
        else if (p5RightRecv == 1) {
          strcat(msg, ",1");
        }
        else if (p5RightRecv == 2) {
          strcat(msg, ",2");
        }
        else if (p5RightRecv == 3) {
          strcat(msg, ",3");
        }
        else if (p5RightRecv == 4) {
          strcat(msg, ",4");
        }
      }


      //      strcat(wirelessArduinoRight, "");
      //      itoa(p5RightRecv, wirelessArduinoRight, 2); // convert integer to string

      //      char myChar = (char)(p5RightRecv + 48);
      //      strcat(msg, myChar);
      //      strcat(msg, p5RightRecv);

      Serial.print(p5LeftSend);
      Serial.print(',');
      Serial.println(p5RightSend);

    }
  }


  radio.stopListening();
  rf24SendData();
  delay(100);

}


void clearData() {
  // set all fields to 0
  data.servoBits = 0;
  data.neoPixelBits = 0;
  data.playbackControlBits = 0;
}

// End of transmitter code
