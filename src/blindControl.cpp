#include <Arduino.h>

uint8_t upPin;
uint8_t downPin;
uint16_t readPin;

const int MINPOS = 20;
const int MAXPOS = 1004;
int openPos = 33;
int closePos = 755;

void setupBlind(uint8_t up, uint8_t down, uint8_t read) {
  upPin = up;
  downPin = down;
  readPin = read;

  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
};
int scaler(double value) { return value * (closePos - openPos) + openPos; }
int blindPosition() { return analogRead(readPin); }
double openRate() {
  double scaledValue =
      ((blindPosition() - openPos) * 1.00 / (closePos - openPos));
  Serial.println(scaledValue);
  return scaledValue;
}

String goToPosition(double target) {
  String message = "";
  if (target < 0.0 || target > 1.0) {
    message = "TARGET INVALID";
  } else {
    int sTarget = scaler(target);
    // Controler:
    int error = blindPosition() - sTarget;

    if (abs(error) > 15) {
      if (error > 0) {
        analogWrite(upPin, 255);
        analogWrite(downPin, 0);
      } else {
        analogWrite(downPin, 255);
        analogWrite(upPin, 0);
      }
    } else {
      analogWrite(downPin, 0);
      analogWrite(upPin, 0);
    }
  }

  // Error position codes:
  if (blindPosition() < MINPOS) {
    analogWrite(upPin, 0);
    message = "MIN LIMIT";
  }
  if (blindPosition() > MAXPOS) {
    analogWrite(downPin, 0);
    message = "MAX LIMIT";
  }

  return message;
}