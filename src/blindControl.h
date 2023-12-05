#include <Arduino.h>

void setupBlind(uint8_t up, uint8_t down, uint8_t read);
int blindPosition();
double openRate();
String goToPosition(double target);