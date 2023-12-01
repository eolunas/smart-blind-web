#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SD.h>
#include <SPI.h>
#include <blindControl.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 20, 15};
byte gateway[] = {192, 168, 20, 1};
byte subnet[] = {255, 255, 255, 0};
EthernetServer server(80);
String HTTP_req;

// Variable for position control
unsigned long readDate_time = 0;
String time = "";

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  server.begin();
  HTTP_req = "";

  // setupBlind(6, 5, A0);
}

void loop() {
  if ((millis() - readDate_time) >= 2000 || readDate_time == 0) {
    readDate_time = millis();
    // time = getCurrentTime();
    Serial.println("no new client!");
  }

  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        Serial.println("new client!");

        StaticJsonDocument<96> doc;

        doc["position"] = blindPosition();
        doc["wake-up"] = "06:00:00";
        doc["go-bed"] = "22:00:00";
        doc["time"] = time;

        Serial.print(F("Sending: "));
        serializeJson(doc, Serial);
        Serial.println();

        // Write response headers
        client.println(F("HTTP/1.0 200 OK"));
        client.println(F("Content-Type: application/json"));
        client.println(F("Connection: close"));
        client.print(F("Content-Length: "));
        client.println(measureJsonPretty(doc));
        client.println();

        // Write JSON document
        serializeJsonPretty(doc, client);
        delay(200);

        client.stop();
      }
    }
  }
}