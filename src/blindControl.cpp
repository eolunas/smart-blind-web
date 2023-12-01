#include <Arduino.h>

uint16_t readPin;

void setupBlind(int upPin, int downPin, uint8_t readPin) {
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  readPin = readPin;
};

int blindPosition() { return analogRead(readPin); }

void changePosition(int pos) {
  //   switch (blackOut) {
  //     case 0:
  //       analogWrite(UP, 0);
  //       analogWrite(DOWN, 0);
  //       break;
  //     case 1:
  //       analogWrite(UP, 255);
  //       analogWrite(DOWN, 0);
  //       break;
  //     case 2:
  //       analogWrite(UP, 0);
  //       analogWrite(DOWN, 255);
  //       break;
  //     default:
  //       analogWrite(UP, 0);
  //       analogWrite(DOWN, 0);
  //       break;
  //   }
}