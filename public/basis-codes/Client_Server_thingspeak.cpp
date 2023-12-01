
//zoomkat 05-01-15, thingspeak, combined client and server
//simple button GET with iframe code
//for use with IDE 1.0
//open serial monitor and send a g to test client GET and
//see what the arduino client/server receives
//web page buttons make pins high/low
//use the ' in html instead of " to prevent having to escape the "
//address will look like http://192.168.1.102:84 when submited
//for use with W5100 based ethernet shields

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //assign arduino mac address
byte ip[] = {
  192, 168, 20, 102 }; // ip in lan assigned to arduino
byte gateway[] = {
  192, 168, 1, 1 }; // internet access via router
byte subnet[] = {
  255, 255, 255, 0 }; //subnet mask
EthernetServer server(84); //server port arduino server will use
//char serverName[] = "checkip.dyndns.com"; // test web page server

char thingSpeakAddress[] = "api.thingspeak.com";
EthernetClient client;
String writeAPIKey = "XXXMX2WYYR0EVZZZ";
String tsData = "hello";  //dummy data
String readString; //used by server to capture POST request 
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;
unsigned long int updateCounter=0; //set refresh counter to 0

//////////////////////

void setup(){

  pinMode(5, OUTPUT); //pin selected to control
  pinMode(6, OUTPUT); //pin selected to control
  pinMode(7, OUTPUT); //pin selected to control
  pinMode(8, OUTPUT); //pin selected to control

  //pinMode(5, OUTPUT); //pin 5 selected to control
  Ethernet.begin(mac,ip,gateway,gateway,subnet); 
  server.begin();
  Serial.begin(9600); 
  Serial.println(F("ThingSpeak server/client 1.0 test 5/01/15")); // keep track of what is loaded
  Serial.println(F("Send a g in serial monitor to test client")); // what to do to test client
}

void loop(){
  // check for serial input
  String analogValue0 = String(analogRead(A0), DEC);

  if (Serial.available() > 0) 
  {
    byte inChar;
    inChar = Serial.read();
    if(inChar == 'g')
    {
      updateThingSpeak("field1="+analogValue0);
      //updateThingSpeak(); // call client sendGET function
    }
  }  

  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    updateThingSpeak("field1="+analogValue0);
  }

  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {
    startEthernet();
  }

  lastConnected = client.connected();

  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.print(readString); //print to serial monitor for debuging 

            //now output HTML data header
          if(readString.indexOf('?') >=0) { //don't send new page
            client.println("HTTP/1.1 204 Zoomkat");
            client.println();
            client.println();  
          }
          else {   
            client.println(F("HTTP/1.1 200 OK")); //send new page on browser request
            client.println(F("Content-Type: text/html"));
            client.println();

            client.println(F("<HTML>"));
            client.println(F("<HEAD>"));
            client.println(F("<TITLE>Arduino GET test page</TITLE>"));
            client.println(F("</HEAD>"));
            client.println(F("<BODY>"));

            client.println(F("<H1>Zoomkat's simple Arduino 1.0 button</H1>"));

            // DIY buttons
            client.println(F("Pin5"));
            client.println(F("<a href=/?on2 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off3 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin6"));
            client.println(F("<a href=/?on4 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off5 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin7"));
            client.println(F("<a href=/?on6 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off7 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pin8"));
            client.println(F("<a href=/?on8 target=inlineframe>ON</a>")); 
            client.println(F("<a href=/?off9 target=inlineframe>OFF</a><br><br>")); 

            client.println(F("Pins"));
            client.println(F("&nbsp;<a href=/?off2468 target=inlineframe>ALL ON</a>")); 
            client.println(F("&nbsp;<a href=/?off3579 target=inlineframe>ALL OFF</a>")); 

            client.println(F("<IFRAME name=inlineframe style='display:none'>"));          
            client.println(F("</IFRAME>"));

            client.println(F("</BODY>"));
            client.println(F("</HTML>"));
          }

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf('2') >0)//checks for 2
          {
            digitalWrite(5, HIGH);    // set pin 5 high
            Serial.println(F("Led 5 On"));
            Serial.println();
          }
          if(readString.indexOf('3') >0)//checks for 3
          {
            digitalWrite(5, LOW);    // set pin 5 low
            Serial.println(F("Led 5 Off"));
            Serial.println();
          }
          if(readString.indexOf('4') >0)//checks for 4
          {
            digitalWrite(6, HIGH);    // set pin 6 high
            Serial.println(F("Led 6 On"));
            Serial.println();
          }
          if(readString.indexOf('5') >0)//checks for 5
          {
            digitalWrite(6, LOW);    // set pin 6 low
            Serial.println(F("Led 6 Off"));
            Serial.println();
          }
          if(readString.indexOf('6') >0)//checks for 6
          {
            digitalWrite(7, HIGH);    // set pin 7 high
            Serial.println(F("Led 7 On"));
            Serial.println();
          }
          if(readString.indexOf('7') >0)//checks for 7
          {
            digitalWrite(7, LOW);    // set pin 7 low
            Serial.println(F("Led 7 Off"));
            Serial.println();
          }     
          if(readString.indexOf('8') >0)//checks for 8
          {
            digitalWrite(8, HIGH);    // set pin 8 high
            Serial.println(F("Led 8 On"));
            Serial.println();
          }
          if(readString.indexOf('9') >0)//checks for 9
          {
            digitalWrite(8, LOW);    // set pin 8 low
            Serial.println(F("Led 8 Off"));
            Serial.println();
          }         

          //clearing string for next read
          readString="";

        }
      }
    }
  }
} 

//////////////////////////
void updateThingSpeak(String tsData)
//void sendGET() //client function to send and receive GET data from external server.
{
  if (client.connect(thingSpeakAddress, 80)) { 
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    lastConnectionTime = millis();
    
    updateCounter++;
    Serial.print("Connecting to ThingSpeak...");
    Serial.println(updateCounter);
    //x=x+1; //page upload counter

    if (client.connected())
    {
      //Serial.println("Connecting to ThingSpeak...");
      //Serial.println();
      failedCounter = 0;
    }
    else {
      failedCounter++;
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
      Serial.println("connection failed");
      Serial.println();
    }
  }
  else
  {
    failedCounter++;
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    lastConnectionTime = millis(); 
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    //readString += c; //places captured byte in readString
    //Serial.print(c); //print raw data
  }

  //Serial.println();
  client.stop(); //stop client
  /* //display captured readString
  Serial.println(F("client disconnected."));
   Serial.println(F("Data from server captured in readString:"));
   Serial.println();
   Serial.print(readString); //prints readString to serial monitor 
   Serial.println();
   Serial.println(F("End of readString"));
   Serial.println(F("=================="));
   Serial.println();
   */
  readString=""; //clear readString variable
}

//////////////////////////
void startEthernet()
{
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  
  delay(1000);

  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Ethernet.begin(mac,ip,gateway,gateway,subnet); 
    server.begin();
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
  }
 delay(1000);
}





