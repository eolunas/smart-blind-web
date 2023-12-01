#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

int HTTP_PORT = 80;
String HTTP_METHOD = "GET";
char HOST_NAME[] = "worldtimeapi.org";
String PATH_NAME = "/api/timezone/America/Bogota";

void requestTime(EthernetClient client) {
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    if (client.println() == 0) {
      Serial.println(F("Failed to send request"));
      client.stop();
      return;
    }
    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      client.stop();
      return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      client.stop();
      return;
    }

    // Allocate the JSON document
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // const char* abbreviation = doc["abbreviation"];  // "-05"
    // const char* client_ip =
    //     doc["client_ip"];  // "2800:484:728a:5400:c8ef:79c7:ff53:6a80"
    // const char* datetime =
    //     doc["datetime"];                   //
    //     "2023-11-22T10:24:03.738563-05:00"
    // int day_of_week = doc["day_of_week"];  // 3
    // int day_of_year = doc["day_of_year"];  // 326
    // bool dst = doc["dst"];                 // false
    // // doc["dst_from"] is null
    // int dst_offset = doc["dst_offset"];  // 0
    // // doc["dst_until"] is null
    // int raw_offset = doc["raw_offset"];      // -18000
    // const char* timezone = doc["timezone"];  // "America/Bogota"
    // long unixtime = doc["unixtime"];         // 1700666643
    // const char* utc_datetime =
    //     doc["utc_datetime"];  // "2023-11-22T15:24:03.738563+00:00"
    // const char* utc_offset = doc["utc_offset"];  // "-05:00"
    // int week_number = doc["week_number"];        // 47

    Serial.println(F("Response:"));
    Serial.println(F("Unixtime:"));
    Serial.println(doc["unixtime"].as<long>());

    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {
    Serial.println("connection failed");
  }
}