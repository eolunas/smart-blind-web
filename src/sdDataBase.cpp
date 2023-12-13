#include <Arduino.h>
#include <SD.h>

char aRecord[10];
byte recordNum;
byte charNum;

void setupDB(uint8_t pin, char (*params)[10]) {
  if (!SD.begin(pin)) {
    return;
  }
  File archivoSD = SD.open("data.txt");
  if (archivoSD) {
    while (archivoSD.available()) {
      char inChar = archivoSD.read();
      if (inChar == '\n') {
        strcpy(params[recordNum], aRecord);
        recordNum++;
        charNum = 0;
      } else {
        aRecord[charNum++] = inChar;
        aRecord[charNum] = '\0';
      }
    }
  }
  archivoSD.close();
}