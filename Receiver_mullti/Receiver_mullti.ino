
// Code 3 : Bidirectional Communication (Receiver)
// Library: TMRh20/RF24 (https://github.com/tmrh20/RF24/)

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(8, 10);  // CE, CSN

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  int yMapped = 0;
  int xMapped = 0;
  int trimwert;
};

Data_Package data;

int in1 = 5;
int in2 = 3;
int in3 = 9;
int in4 = 6;

const byte addresses[][10] = { "ADDRESS01", "ADDRESS02" };

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);     // ADDRESS02
  radio.openReadingPipe(1, addresses[0]);  // ADDRESS01
  radio.setPALevel(RF24_PA_MAX);
};

void loop() {
  delay(10);

  radio.startListening();
  if (radio.available()) {  // Receive
                            //char txt_received[10] = "";
    radio.read(&data, sizeof(Data_Package));
    //int leftPWM = 0;
    //radio.read(&leftPWM, sizeof(leftPWM));
    //Serial.print("yMapped: ");
    //Serial.print(data.yMapped);
    // Serial.print(" xMapped: ");
    // Serial.println(data.xMapped);
  };

  // Daten für Bewegungsrichtung und Geschwindigkeit
  int speed = abs(data.yMapped);

  int speedx = abs(data.xMapped);
  int direction = 0;
  //Serial.println(speed);
  //if ((data.xMapped <= 130 && data.xMapped >= 120) && (data.yMapped <= 130 && data.yMapped >= 120)) {
  // analogWrite(in1, 0);
  //  analogWrite(in2, 0);
  //   analogWrite(in3, 0);
  //   analogWrite(in4, 0);
  //  } else
  if (data.yMapped <= 120) {
    //Serial.println(data.xMapped);
    //Rückwärts
    analogWrite(in1, speed);
    analogWrite(in2, 0);
    analogWrite(in3, 0);
    analogWrite(in4, speed);
  } else if (data.yMapped >= 130 && data.yMapped <= 255) {
    // Vorwärts
    analogWrite(in1, 0);
    analogWrite(in2, speed + data.trimwert);
    analogWrite(in3, speed);
    analogWrite(in4, 0);
  }

  if (data.xMapped > 10 && data.yMapped > 10) {
    // Rechts drehen
    //erial.println("Test");
    analogWrite(in1, 0);
    analogWrite(in2, (speedx / 3) + data.trimwert);
    analogWrite(in3, speedx);
    analogWrite(in4, 0);

  } else if (data.xMapped <= -10 && data.yMapped > 10) {
    // Links drehen

    analogWrite(in1, 0);
    analogWrite(in2, speedx);
    analogWrite(in3, (speedx / 3) + data.trimwert);
    analogWrite(in4, 0);
  }
  Serial.println(data.trimwert);
}


//radio.stopListening();                  // Transmit
//const char txt_sent[] = "Sender 2";
//radio.write(&txt_sent, sizeof(txt_sent));
//Serial.print("Sent to: ");Serial.println(txt_sent);
