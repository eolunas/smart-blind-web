#include <Arduino.h>

uint8_t upPin;
uint8_t downPin;
uint8_t readPin;

int openPos;
int closePos;

void setupBlind(uint8_t up, uint8_t down, uint8_t read, int open, int close) {
  upPin = up;
  downPin = down;
  readPin = read;
  openPos = (open > 20 && open < 1000) ? open : 33;
  closePos = (close > 20 && close < 1000) ? close : 750;

  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
}

uint8_t openRate() {
  double value =
      ((analogRead(readPin) - openPos) * 1.0) / ((closePos - openPos) * 1.0);
  return 100 * value;
}

uint8_t moveTo(uint8_t target) {
  uint8_t errCode = 0;
  if (target < 0 || target > 100) {
    errCode = 1;
  } else {
    int error = openRate() - target;
    if (abs(error) > 2) {
      if (error > 0) {
        digitalWrite(upPin, HIGH);
        digitalWrite(downPin, LOW);
      } else {
        digitalWrite(downPin, HIGH);
        digitalWrite(upPin, LOW);
      }
    } else {
      digitalWrite(downPin, LOW);
      digitalWrite(upPin, LOW);
    }
  }

  if (analogRead(readPin) < 20) {
    digitalWrite(upPin, LOW);
    errCode = 2;
  }
  if (analogRead(readPin) > 1000) {
    digitalWrite(downPin, LOW);
    errCode = 3;
  }

  return errCode;
}