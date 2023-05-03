#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(8, 10);  // CE, CSN


// Max size of this struct is 32 bytes - NRF24L01 buffer limit

struct TransmitterData {
  int yMapped = 0;
  int xMapped = 0;
  int trimValue = 0;
  int userPinCode;
  int PinCode

};

struct ReceiverData {
  int distance;
};

TransmitterData txData;
ReceiverData rxData;

//motor pins
int in1 = 5;
int in2 = 3;
int in3 = 9;
int in4 = 6;

const int breakLights = 7; // break lights

// ultra sonic sensor
const int trigPin = 8;  
const int echoPin = 7;
int trigState = LOW;   //state of trigPin
int interval = 1;      // interval in milliseconds at which trigPin turns on
int interval2 = 1000;  //time in milliseconds at which the distance is printed in serial monitors
int printState = LOW;  //whether or not to print distance
unsigned long previousMillis = 0;

//
int motorSpeedA = 0;
int motorSpeedB = 0;


const byte addresses[][10] = { "ADDRESS01", "ADDRESS02" };

int measureDistance() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (trigState == LOW) {
      trigState = HIGH;
    } else {
      trigState = LOW;
    }
  }

  digitalWrite(trigPin, trigState);

  int duration, distance;
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  //if (printState == HIGH) {
  // Serial.print(distance);
  //Serial.println("cm");
  //}
  return distance;
}

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);     // ADDRESS02
  radio.openReadingPipe(1, addresses[0]);  // ADDRESS01
  radio.setPALevel(RF24_PA_MAX);
}

void loop() {



  while(txdata.userPinCode == txtdate.pinCode){
  int distance = measureDistance();
  radio.startListening();

  // Receive
  if (radio.available()) {
    radio.read(&txData, sizeof(TransmitterData));
  }

  if (distance >= 30) {
    //forward
    if (txData.yMapped > 10) {

      motorSpeedA = abs(txData.yMapped + txData.trimValue);
      motorSpeedB = abs(txData.yMapped - txData.trimValue);
      analogWrite(in1, motorSpeedA);
      analogWrite(in2, 0);
      analogWrite(in3, motorSpeedB);
      analogWrite(in4, 0);

      digitalWrite(breakLights, LOW);
    } else {

      //Stop
      motorSpeedA = 0;
      motorSpeedB = 0;
      digitalWrite(breakLights, HIGH);
    }
    //backward
    if (txData.yMapped < -10) {
      motorSpeedA = abs(txData.yMapped + txData.trimValue);
      motorSpeedB = abs(txData.yMapped - txData.trimValue);
      analogWrite(in1, 0);
      analogWrite(in2, motorSpeedA);
      analogWrite(in3, 0);
      analogWrite(in4, motorSpeedB);
      digitalWrite(breakLights, LOW);
    } else if (distance <= 30 && distance>= 10 ) {
      motorSpeedA = map(distance, 0, 30, 0, 255);
      motorSpeedB = map(distance, 0, 30, 0, 255);
      analogWrite(in1, motorSpeedA);
      analogWrite(in2, 0);
      analogWrite(in3, motorSpeedB);
      analogWrite(in4, 0);
    } else if(distance <= 10) {
      motorSpeedA = map(distance, 0, 30, 0, 255);
      motorSpeedB = map(distance, 0, 30, 0, 255);
      analogWrite(in1, 0);
      analogWrite(in2, motorSpeedA);
      analogWrite(in3, 0);
      analogWrite(in4, motorSpeedB);
    }

    //right
    if (txData.xMapped > 10) {
      motorSpeedA = constrain(motorSpeedA + abs(txData.xMapped), 0, 255);
      motorSpeedB = constrain(motorSpeedB - abs(txData.xMapped), 0, 255);
      analogWrite(in1, motorSpeedA);
      analogWrite(in2, 0);
      analogWrite(in3, motorSpeedB);
      analogWrite(in4, 0);
    }

    //left
    if (txData.xMapped < -10) {
      motorSpeedA = constrain(motorSpeedA - abs(txData.xMapped), 0, 255);
      motorSpeedB = constrain(motorSpeedB + abs(txData.xMapped), 0, 255);

      analogWrite(in1, motorSpeedA);
      analogWrite(in2, 0);
      analogWrite(in3, motorSpeedB);
      analogWrite(in4, 0);
    }

    Serial.print("A: ");
    Serial.print(motorSpeedA);
    Serial.print("      ");
    Serial.print("B: ");
    Serial.println(motorSpeedB);


    radio.stopListening();// Transmit
    radio.write(&txData, sizeof(TransmitterData));
    //Serial.print("Sent to: ");Serial.println(txData);
  }
  }
}