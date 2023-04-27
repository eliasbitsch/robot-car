
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(8, 9);          // CE, CSN
const byte addresses[][10] = {"ADDRESS01", "ADDRESS02"};
int xPin = A0; // Joystick X-Achse
int yPin = A1; // Joystick Y-Achse
int xVal, yVal;
int eingang = A2;
int wert = 0;

struct Data_Package {
  int yMapped;
  int xMapped;
  int trimwert;
};
Data_Package data;

void setup() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[0]);    // ADDRESS01
  radio.openReadingPipe(1, addresses[1]); // ADDRESS02
  radio.setPALevel(RF24_PA_MAX);
};

void loop() {
  delay(10);

  radio.stopListening();
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);

  data.yMapped = map(yVal, 0, 1023, -255, 255);
  data.xMapped = map(xVal, 0, 1023, -255, 255);

  radio.write(&data, sizeof(Data_Package));
  Serial.print("y-Achse: ");
  Serial.print(data.yMapped);
  Serial.print("  x-Achse: ");
  Serial.println(data.xMapped);

  wert = analogRead(eingang);
  data.trimwert = map (wert, 0, 1023, -255, 255);
  Serial.println(data.trimwert);

  delay(10);
};
