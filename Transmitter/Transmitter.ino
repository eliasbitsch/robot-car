#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

RF24 radio(8, 9);                                           // CE, CSN
const byte addresses[][10] = { "ADDRESS01", "ADDRESS02" };  // Channel radio module
const int xPin = A0;                                        // Joystick X-Axis
const int yPin = A1;                                        // Joystick Y-Axis
const int trimPin = A2;                                     // Trim with poti
const int joystickButton = 6;                               // Joystick button
const int buzzer = 5;                                       // Buzzer honk
const int headLights = 7;                                   // white LEDs
const int LDR = A3;                                         // Ligh depended resistor

// data to send
struct TransmitterData {
  int yMapped;
  int xMapped;
  int trimValue;
  int userPinCode;
  int pinCode = 1234;
};

// data to receive
struct ReceiverData {
  int distance;
};

TransmitterData txData;
ReceiverData rxData;

void setup() {

if (Serial.available() > 0) {
    // read the incoming data:
    txdata.userPinCode = Serial.read();
  }

  // initializing pins
  pinMode(headLights, OUTPUT);
  pinMode(joystickButton, INPUT_PULLUP);

  // communication with serial monitor
  Serial.begin(9600);

  // communication with radio module
  radio.begin();
  radio.openWritingPipe(addresses[0]);     // ADDRESS01
  radio.openReadingPipe(1, addresses[1]);  // ADDRESS02
  radio.setPALevel(RF24_PA_MAX);
};

void loop() {

  // Receive data
  radio.startListening();
  if (radio.available()) {
    radio.read(&rxData, sizeof(ReceiverData));
  }

  // Read brightness
  int brightness = readBrightnessValue();
  switchLED(brightness);

  // Activate honk if joystick button is pressed
  honk(joystickButton);

  // Map joystick axis range to motor range
  txData.yMapped = map(analogRead(xPin), 0, 1023, -230, 230);
  txData.xMapped = map(analogRead(yPin), 0, 1023, -255, 255);

  // Map trim value to -100, 100
  txData.trimValue = map(analogRead(trimPin), 0, 1023, -100, 100);


  // Transmit data
  radio.stopListening();
  radio.write(&txData, sizeof(TransmitterData));

  // Print data to serial monitor
  Serial.print("Brightness: ");
  Serial.print(brightness);
  Serial.print(" trim: ");
  Serial.print(txData.trimValue);
  Serial.print(" y-axis: ");
  Serial.print(txData.yMapped);
  Serial.print("  x-axis: ");
  Serial.println(txData.xMapped);
}

// Read brightness
int readBrightnessValue() {
  int brightness = map(analogRead(LDR), 850, 1000, 0, 100);
  return brightness;
}

// Switch LED
void switchLED(int brightness) {
  if (brightness > 30) {
    digitalWrite(headLights, HIGH);
  } else {
    digitalWrite(headLights, LOW);
  }
}

// Honk
void honk(int joystickButton) {
  if (joystickButton == HIGH) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
