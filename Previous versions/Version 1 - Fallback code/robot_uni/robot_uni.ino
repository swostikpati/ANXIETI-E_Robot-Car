#include <Servo.h>

//motors for wheels
const int ain1Pin = 2;
const int ain2Pin = 4;
const int pwmAPin = 5;
const int bin1Pin = 8;
const int bin2Pin = 7;
const int pwmBPin = 6;

//servo
const int servoPin = A1;
Servo myservo;

//ultrasonic distance sensor
const int trigPin = A2; //cause analog write has anyways been deactivated in this pin - 10
const int echoPin = A3; // 12
const float speedOfSound = 343.2;
const float alpha = 0.7; //filter parameter
float filtered_distance = 0;
const int collision_threshold = 50;

//IR Distance sensor
#define SENS1_PIN A4
#define SENS2_PIN A5
int SENS1_val;
int SENS2_val;

int pwnVal = 150;

unsigned long previousTime = 0;

// custom delay function
bool myDelay(unsigned long delayTime) {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= delayTime) {
    previousTime = currentTime;
    return true;
  }
  return false;
}

void pingDistance(bool filterFlag) {
  // Send a 10us pulse to the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the pulse from the echo pin
  float duration = pulseIn(echoPin, HIGH);

  // Calculate the distance based on the duration
  float distance = duration * speedOfSound / 2 / 10000; // in cm

  if (filterFlag) {
    // Apply EMA filter
    filtered_distance = (1 - alpha) * filtered_distance + alpha * distance;

  }
  else {
    filtered_distance = distance;

  }


}

bool edgeDetect() {
  SENS1_val = digitalRead(SENS1_PIN);
  SENS2_val = digitalRead(SENS2_PIN);
  return (SENS1_val || SENS2_val);
}

void forward() {
  analogWrite(pwmAPin, pwnVal);
  digitalWrite(ain1Pin, HIGH);
  digitalWrite(ain2Pin, LOW);
  analogWrite(pwmBPin, pwnVal);
  digitalWrite(bin1Pin, LOW);
  digitalWrite(bin2Pin, HIGH);
}

void backward() {
  analogWrite(pwmAPin, pwnVal);
  digitalWrite(ain1Pin, LOW);
  digitalWrite(ain2Pin, HIGH);
  analogWrite(pwmBPin, pwnVal);
  digitalWrite(bin1Pin, HIGH);
  digitalWrite(bin2Pin, LOW);
}

void left() {
  analogWrite(pwmAPin, 0);
  analogWrite(pwmBPin, pwnVal);
  digitalWrite(bin1Pin, LOW);
  digitalWrite(bin2Pin, HIGH);
}

void right() {
  analogWrite(pwmAPin, pwnVal);
  digitalWrite(ain1Pin, HIGH);
  digitalWrite(ain2Pin, LOW);
  analogWrite(pwmBPin, 0);
}

void stopMoving() {
  analogWrite(pwmAPin, 0);
  analogWrite(pwmBPin, 0);

}


//servo movement
int peekLeft()
{
  myservo.write(50);
  //  delay(500);
  pingDistance(false);
  int returnVal = filtered_distance;
  while (!myDelay(500)) {}
  //  myservo.write(115);
  Serial.println("PeekedLeft");
  return returnVal;
}

int peekRight()
{
  myservo.write(170);
  //  delay(500);
  pingDistance(false);
  int returnVal = filtered_distance;
  while (!myDelay(500)) {}
  myservo.write(115);
  Serial.println("PeekedRight");
  return returnVal;
}

void handleCollision() {
  while (true) {
    stopMoving();
    while (!myDelay(700)) {
      backward();
    }
    int leftD = peekLeft();
    int rightD = peekRight();
    Serial.print("Left: ");
    Serial.println(leftD);
    Serial.print("Right: ");
    Serial.println(rightD);
    int betterSide = max(leftD, rightD);

    if (betterSide > collision_threshold) {
      if (betterSide == leftD) {
        Serial.println("Turning left");
        while (!myDelay(700)) {
          left();
        }
        break;
      }
      else if (betterSide == rightD) {
        Serial.println("Turning right");
        while (!myDelay(700)) {
          right();
        }
        break;
      }
    }
  }

}

// Wireless Communication
const int CEPIN = 9;
const int CSNPIN = 10;

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(CEPIN, CSNPIN);  // CE, CSN

const byte addr = 0x76;
const int RF24_CHANNEL_NUMBER = 0;

const byte xmtrAddress[] = { addr, addr, 0xC7, 0xE6, 0xCC };
const byte rcvrAddress[] = { addr, addr, 0xC7, 0xE6, 0x66 };

const int RF24_POWER_LEVEL = RF24_PA_LOW;
uint8_t pipeNum;
unsigned int totalTransmitFailures = 0;

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

  //  Serial.print(F(" ... "));
  if (retval) {
    Serial.println(F("success"));
  } else {
    totalTransmitFailures++;
    Serial.print(F("failure, total failures = "));
    Serial.println(totalTransmitFailures);
  }
}
void setupRF24receiver() {
  setupRF24Common();
  // Set us as a receiver
  radio.openWritingPipe(rcvrAddress);
  radio.openReadingPipe(1, xmtrAddress);
  // radio.printPrettyDetails();
  Serial.println(F("I am a receiver"));
}

void setup() {
  Serial.begin(9600);

  //  for motors
  pinMode(ain1Pin, OUTPUT);
  pinMode(ain2Pin, OUTPUT);
  pinMode(pwmAPin, OUTPUT);

  //for servo
  myservo.attach(servoPin);
  myservo.write(115);

  //  for ultrasonic distance sensor trig and echo
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //  for IR distance sensor
  pinMode(SENS1_PIN, INPUT);
  pinMode(SENS2_PIN, INPUT);

  //  setting up receiver wireless communication
  setupRF24receiver();
}


void loop()
{
  rf24RecvData();
  int moveMsg = atoi(&msg[0]);
  int dirMsg = atoi(&msg[2]);
  Serial.print("moveMsg");
  Serial.println(moveMsg);
  Serial.print("dirMsg");
  Serial.println(dirMsg);



  //strcmp(moveMsg, "1") == 0
  if (moveMsg == 1) {
    forward();
    pingDistance(true);
    Serial.print("Distance to nearest object: ");
    Serial.println(filtered_distance);
    if (filtered_distance < collision_threshold || edgeDetect()) {
      Serial.println("Collision ahead");
      //    while (!myDelay(1000)) {}
      //    handle collision
      handleCollision();

    }
    else {
      while (!myDelay(100)) {}
    }
  }
  else if (moveMsg == 2) {
    if (dirMsg == 0) {
      stopMoving();
    }
    else if (dirMsg == 1) {
      forward();
    }
    else if (dirMsg == 2) {
      backward();
    }
    else if (dirMsg == 3) {
      right();
    }
    else if (dirMsg == 4) {
      left();
    }

  }
  else if (moveMsg == 0) {
    stopMoving();
  }


}
