#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// Initialize the Ethernet server library
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 177);
EthernetServer server(80);
File webPage;
String HTTP_req;

// Variable for position control
int POS;
int blackOut = 0;
int UP = 6;
int DOWN = 5;

// NTP server for time:
EthernetUDP Udp;
unsigned int localPort = 8888;
unsigned long readDate_time = 0;
const char timeServer[] = "time.nist.gov"; 
const unsigned long offsetTime = 18000; //Bogota offset
const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[NTP_PACKET_SIZE]; 
unsigned long epoch;
String currentTime;

void setup(){
  // Initialize SD card
  Serial.begin(9600);       // for debugging
  Serial.println("Checking SD card is accessible...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");

  //Ethernet start
  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  HTTP_req = "";

  //UDP Ethernet start
  Udp.begin(localPort);

  // PWM control motor from H Bridge:
  pinMode(DOWN, OUTPUT);
  pinMode(UP, OUTPUT);

}
void loop(){
  if ((millis() - readDate_time) >= 60000 || readDate_time == 0) {
    readDate_time = millis();
    clientFunctions();
  }
  serverFunctions();
}

// Client code
void clientFunctions() {
  if (sendNTPpacket(timeServer)) {
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900) subtract offet of Location:
    epoch = (highWord << 16 | lowWord) - offsetTime;
    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    currentTime = String((epoch  % 86400L) / 3600) + ":";
    if (((epoch % 3600) / 60) < 10) currentTime += "0";
    currentTime += String((epoch  % 3600) / 60) + ":"; // print the minute (3600 equals secs per minute)
    if ((epoch % 60) < 10) currentTime += "0";
    currentTime += String(epoch % 60);
    Serial.println(currentTime);
  }
}
int sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  delay(1000);
  return Udp.parsePacket();
}
// Server code
void serverFunctions() {
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if ( HTTP_req.length() < 80) HTTP_req += c;  // save the HTTP request 1 char at a time
        if (c == '\n' && currentLineIsBlank) {
          Serial.println(HTTP_req);
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

        if (HTTP_req.indexOf("ajaxrefresh") >= 0 ) {
            ajaxRequest(client);  //update the analog values
            break;
          } else if (HTTP_req.indexOf("isBlackOutUp") >= 0 ) {
              changePosition(client, 0); //change the LED state
              break;
          } else if (HTTP_req.indexOf("isBlackOutDown") >= 0 ) {
              changePosition(client, 1); //change the LED state
              break;
          } else {
              webPage = SD.open("index.htm"); // open web page file
              if (webPage) {
                while (webPage.available()) {
                  client.write(webPage.read()); // send web page to client
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
    Serial.println("Client disconnected");
  } // end if (client)
}
void ajaxRequest(EthernetClient client){
    POS = analogRead(A0);
    client.print("Current position: ");
    client.print(POS);
    client.println("<br />");
    client.print("Time: ");
    client.print(epoch);
    client.println("<br />");
}
void changePosition(EthernetClient client, int pos){
  if (pos == 0) {
    blackOut = 0;
    client.print("moving up");
  } else {
    blackOut = 1;
    client.print("moving down");
  }

  switch (blackOut){
    case 0:
      analogWrite(UP, 0);
      analogWrite(DOWN, 0);
      break;
    case 1:
      analogWrite(UP, 255);
      analogWrite(DOWN, 0);
      break;
    case 2:
      analogWrite(UP, 0);
      analogWrite(DOWN, 255);
      break;
    default:
      analogWrite(UP, 0);
      analogWrite(DOWN, 0);
      break;
  } 
}
