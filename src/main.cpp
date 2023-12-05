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
String HTTP_req = "";

// Variable for position control
unsigned long readDate_time = 0;
double target;  // Value between [0.0, 1.0]
String time = "";

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  server.begin();

  setupBlind(6, 5, A0);
  target = openRate();
}

void loop() {
  // Servo functions with securuty limits:
  String message = goToPosition(target);

  if ((millis() - readDate_time) >= 10000 || readDate_time == 0) {
    readDate_time = millis();
    // time = getCurrentTime();
    // Serial.println("no new client!");
  }

  EthernetClient client = server.available();
  if (client) {
    HTTP_req = "";
    while (client.connected()) {
      if (client.available()) {
        Serial.println("new client!");

        // Read the request
        char c = '0';
        while (c != '\n') {
          c = client.read();
          if (HTTP_req.length() < 100) {
            HTTP_req += c;
          }
        }

        // Readd the target and asign it
        int index = HTTP_req.indexOf("target=");
        if (index > 0) {
          target = HTTP_req.substring(index + 7, index + 11).toDouble();
        }

        StaticJsonDocument<96> doc;

        doc["position"] = openRate();
        doc["wake-up"] = "06:30:00";
        doc["go-bed"] = "22:00:00";
        doc["time"] = time;
        doc["message"] = message;

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