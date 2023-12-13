#include <Ethernet.h>
#include <SD.h>
#include <SPI.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 20, 177);
// Initialize the Ethernet server library
// (port 80 is default for HTTP):
EthernetServer server(80);
File webPage;
String HTTP_req;

// Variable for position control
int UP = 6;
int DOWN = 5;
long position;
int readDate_time = 0;

// NTP server for time:
const char timeServer[] = "time.nist.gov";  // time.nist.gov NTP server
const int NTP_PACKET_SIZE =
    48;  // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];  // buffer to hold incoming and outgoing
                                     // packets

void setup() {
  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  Serial.begin(9600);       // for debugging

  // initialize SD card
  Serial.println("Checking SD card is accessible...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;  // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");

  HTTP_req = "";

  // PWM control for H Bridge:
  pinMode(DOWN, OUTPUT);
  pinMode(UP, OUTPUT);
}

// send the state of the switch to the web browser
void sendPos(EthernetClient client) {
  int sensorReading = analogRead(A0);
  client.print("Current position: ");
  client.print(sensorReading);
  client.println("<br />");
}

void ledChangeStatus(EthernetClient client) {
  int state = digitalRead(6);
  Serial.println(state);
  if (state == 1) {
    digitalWrite(6, LOW);
    client.print("OFF");
  } else {
    digitalWrite(6, HIGH);
    client.print("ON");
  }
}

void closeBlackOut() {
  analogWrite(UP, 0);
  analogWrite(DOWN, 255);
}

void openBlackOut() {
  analogWrite(DOWN, 0);
  analogWrite(UP, 255);
}

void stopBlackOut() {
  analogWrite(UP, 0);
  analogWrite(DOWN, 0);
}

void loop() {
  if ((millis() - readDate_time) >= 10000 || readDate_time == 0) {
    readDate_time = millis();
    // time = getCurrentTime();
  }
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (HTTP_req.length() < 80)
          HTTP_req += c;  // save the HTTP request 1 char at a time

        if (c == '\n' && currentLineIsBlank) {
          Serial.println(HTTP_req);

          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          if (HTTP_req.indexOf("ajaxrefresh") >= 0) {
            sendPos(client);  // update the analog values
            break;
          } else if (HTTP_req.indexOf("ledstatus") >= 0) {
            ledChangeStatus(client);  // change the LED state
            break;
          } else {
            webPage = SD.open("index.htm");  // open web page file
            if (webPage) {
              while (webPage.available()) {
                client.write(webPage.read());  // send web page to client
              }
              webPage.close();
            }
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    HTTP_req = "";
    Serial.println("client disconnected");
  }  // end if (client)
}