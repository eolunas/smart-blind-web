#include <ArduinoJson.h>
#include <Ethernet.h>
#include <RTClib.h>
#include <blindControl.h>
#include <sdDataBase.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 20, 15};
byte gateway[] = {192, 168, 20, 1};
byte subnet[] = {255, 255, 255, 0};
EthernetServer server(80);
String HTTP_req = "";

// Variable SD storage
char params[4][10];
long prevMillis = 0;
RTC_DS3231 rtc;

// Variable for position control
uint8_t target;
char time[9] = "00:00:00";

void setup() {
  rtc.begin();
  setupDB(4, params);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  server.begin();
  setupBlind(6, 5, A0, atoi(params[2]), atoi(params[3]));
  target = openRate();
}

void loop() {
  // Servo functions with securuty limits:
  uint8_t code = moveTo(target);

  EthernetClient client = server.available();
  if (client) {
    HTTP_req = "";
    while (client.connected()) {
      if (client.available()) {
        char c = '0';
        while (c != '\n') {
          c = client.read();
          if (HTTP_req.length() < 100) {
            HTTP_req += c;
          }
        }

        // Read the target and asign it
        int index = HTTP_req.indexOf("target=");
        if (index > 0) {
          target = (uint8_t)HTTP_req.substring(index + 7, index + 11).toInt();
          client.println(F("HTTP/1.0 200 OK"));
          client.println(F("Connection: close"));
        } else {
          StaticJsonDocument<96> doc;

          doc["position"] = openRate();
          doc["wake-up"] = params[0];
          doc["go-bed"] = params[1];
          doc["time"] = time;
          doc["message"] = code;

          client.println(F("HTTP/1.0 200 OK"));
          client.println(F("Content-Type: application/json"));
          client.println(F("Connection: close"));
          client.print(F("Content-Length: "));
          client.println(measureJsonPretty(doc));
          client.println();

          serializeJsonPretty(doc, client);
          delay(300);
        }

        client.stop();
      }
    }
  }
}