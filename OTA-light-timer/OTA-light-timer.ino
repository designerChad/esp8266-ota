/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
  Arduino IDE example: Examples > Arduino OTA > BasicOTA.ino
*********/
// Both of these are included when you pick the Time library in Arduino IDE library manager
//https://github.com/michaelmargolis/arduino_time/tree/master/Time

#include <Time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h> //horack
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "/Users/chadwickanderson/documents/arduino/Includes/networkheader"

// ###### OTA #################################
// Replace with your network credentials
//const char* ssid = "*****";
//const char* password = "*****";
//const int ESP_BUILTIN_LED = 2;
// ########################################

// ###### WEBSERVER SETUP #################################
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;



// ###### TIME STUFF #################################
// all from Horack
bool firstTimeGot = false;
String timeStr = "NO-TIME-SET";
String dateStr = "NO-DATE-SET";
// the following 2 ints are just for diagnostic purposes to see how many times we tried to reach NPT and succeeded (and how far)
int nptGets = 0;
int nptAttempts = 0;
// ########################################

// #### LED SETUP #########################
#define interval 5000

#include "FastLED.h"
#define NUM_LEDS 22 //155

struct CRGB leds3[NUM_LEDS];
struct CRGB leds4[NUM_LEDS];
struct CRGB leds5[NUM_LEDS];
struct CRGB leds6[NUM_LEDS];

uint8_t gBrightness = 180;
#define UPDATES_PER_SECOND 100
#define FRAMES_PER_SECOND  120

// ########################################

void setup() {
  //fill_solid( leds6, NUM_LEDS, CHSV(0,0,0));
  FastLED.clear();
  //digitalWrite(6, LOW);
// ###### OTA ESSENTIALS ######################################  
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
   ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
   ArduinoOTA.setHostname("ChadzESP8266-2");

  // No authentication by default
   ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(ESP_BUILTIN_LED, OUTPUT);

// ######  / OTA ESSENTIALS ###################################### 

  
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);




// #### LED CODE BEGIN #######
    FastLED.addLeds<WS2812B,3,GRB>(leds3, NUM_LEDS).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,4,GRB>(leds4, NUM_LEDS).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,5,GRB>(leds5, NUM_LEDS).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,6,GRB>(leds6, NUM_LEDS).setCorrection(TypicalLEDStrip); // TALL ARM

  //Webserver startup
  server.begin();

} //setup

uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void loop() {
  ArduinoOTA.handle();

  updateTimeFromServer(); // HORACK - sync our internal clock periodically with real time from remote server (also gets actual time rather than relative from boot time)

// #### WEBSERVER / CLIENT #################################################
  
WiFiClient client = server.available();   // Listen for incoming clients
  //delay(15000); // delay to give the NTP server time to fetch correct time

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

              client.println("<p> Hello World!</p>");
              client.println("<p>");
                client.println( year() );
              client.println("</p>");
              //day(), month(), year(), hour(), minute(), second()
              client.println("<p>Day :: ");
                client.println( day() );
              client.println("</p>");

              client.println("<p>Month ::");  
                client.println( month() ); client.println("</p>");
              client.println("<p>Year :: "); 
                client.println( year() ); 
              client.println("</p>");
              client.println("<p>Hour :: "); 
                client.println( hour() ); 
              client.println("</p>");
              client.println("<p>Minute :: "); 
                client.println( minute() ); 
              client.println("</p>");
              client.println("<p>Second :: "); 
                client.println( second() ); 
              client.println("</p>");

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }


// #### / WEBSERVER / CLIENT #################################################

  //digitalWrite(ESP_BUILTIN_LED, LOW);
  //delay(200);
  //digitalWrite(ESP_BUILTIN_LED, HIGH);
  //delay(200);

// #### LED LOOP #################################################
//fill_solid( leds3, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds4, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds5, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds6, NUM_LEDS, CHSV(0,0,0));
//
//  FastLED.show();
//
//
//     delay(2000);
//  fill_solid( leds3, NUM_LEDS,CHSV(HUE_BLUE,255,255));
//  fill_solid( leds4, NUM_LEDS,CHSV(HUE_BLUE,255,255));
//  fill_solid( leds5, NUM_LEDS,CHSV(HUE_BLUE,255,255));
//  fill_solid( leds6, NUM_LEDS,CHSV(HUE_BLUE,255,255));
//  
//        FastLED.show();
//   delay(2000);
//  fill_solid( leds3, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds4, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds5, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds6, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//
//        FastLED.show();
//   delay(2000);
//        
//
//        fill_solid( leds3, NUM_LEDS, CHSV(HUE_RED,255,255));
//        fill_solid( leds4, NUM_LEDS, CHSV(HUE_RED,255,255));
//        fill_solid( leds5, NUM_LEDS, CHSV(HUE_RED,255,255));
//        fill_solid( leds6, NUM_LEDS, CHSV(HUE_RED,255,255));
//
//        FastLED.show();
//   delay(2000);
//
//         fill_solid( leds3, NUM_LEDS, CHSV(HUE_GREEN,255,255));
//         fill_solid( leds4, NUM_LEDS, CHSV(HUE_GREEN,255,255));
//         fill_solid( leds5, NUM_LEDS, CHSV(HUE_GREEN,255,255));
//         fill_solid( leds6, NUM_LEDS, CHSV(HUE_GREEN,255,255));
//
//         FastLED.show();
//   delay(2000);

  //rainbow();
  //fill_solid( leds6, NUM_LEDS, CHSV(HUE_RED,255,255));
  
   EVERY_N_MILLISECONDS( 100 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);


// Timer functions to change color of LEDs
  //EVERY_N_MILLISECONDS( 100 ) { 
    //changeColor ();
  //}

  // if( minute() == 36 ){
  //     //digitalWrite(6, HIGH);
  //     fill_solid( leds6, NUM_LEDS, CHSV(HUE_BLUE,255,255));
  // }
  // if( minute() == 37 ){
  //     fill_solid( leds6, NUM_LEDS, CHSV(0,0,0));
  //     //digitalWrite(6, LOW);
  //     //fill_solid( leds6, NUM_LEDS, CHSV(HUE_GREEN,255,255));
  // }
  //  if( minute() == 38 ){
  //     //digitalWrite(6, LOW);
  //     fill_solid( leds6, NUM_LEDS, CHSV(HUE_ORANGE,255,255));
  // }
  // if( minute() >= 43 ){
  //     //digitalWrite(6, HIGH);
  //     rainbow();
  // }

  if( minute() % 2 == 0){ // flip flop every other minute
    fill_solid( leds6, NUM_LEDS, CHSV(HUE_BLUE,255,255));
  }else{
    //fill_solid( leds6, NUM_LEDS, CHSV(HUE_BLUE,255,255));
    rainbow();
  }
  
  if ( ( hour() > 16 ) && ( hour() <= 22 ) ) { // Run between 6pm and 11pm
      FastLED.show();
  } else if ( ( hour() == 22 ) && ( minute() < 10 ) ) { // Go until 10min past 10pm
    FastLED.show();
  } else {
      FastLED.clear();
  }

} // loop

// ############## FUNCTIONS ################

void changeColor(){
  if( minute() == 7 ){
      digitalWrite(6, HIGH);
      fill_solid( leds6, NUM_LEDS, CHSV(HUE_BLUE,255,255));
  }
  if( minute() == 8 ){
      fill_solid( leds6, NUM_LEDS, CHSV(0,0,0));
      digitalWrite(6, LOW);
  }
   if( minute() == 9 ){
      digitalWrite(6, LOW);
      fill_solid( leds6, NUM_LEDS, CHSV(HUE_ORANGE,255,255));
  }
  if( minute() == 10 ){
      digitalWrite(6, HIGH);
      rainbow();
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds6, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(90);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds6[ random16(NUM_LEDS) ] += CRGB::White;
  }
}



// ##### HORACK TIME FUNCTIONS ##################################
// NPT time server stuff ********************************************************************************
const long timeInterval = 60*60*1000;   // interval at which to read time webpage (hourly)
unsigned long previousTimeMillis = timeInterval;    // will store last time was read

void updateTimeFromServer() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousTimeMillis >= timeInterval) {
    // save the last time you read the server time 
    previousTimeMillis = currentMillis;
    nptAttempts++;
    if (setNTPtime() || firstTimeGot) {
      previousTimeMillis = currentMillis;
      firstTimeGot = true;
    } else {
      previousTimeMillis = currentMillis - timeInterval + (30*1000); // if failed, try again in 30 seconds
    }
  }
  dateStr = String(dayShortStr(weekday())) + ", " + String(monthShortStr(month())) + " " + String(day());
  String secStr = "";
// HORACK OUTPUT TO TFT - DOES NOT APPLY TO ME
//#ifndef USE_TFT
//  secStr = ":" + zeroPad(second(), 2); // add seconds in only if NOT tft because it causes too much flashing (need to figure out double buffering)
//#endif
//  timeStr = zeroPad(hourFormat12(), 2) + ":" + zeroPad(minute(), 2) + secStr + " " + (isAM() ? "AM" : "PM");

// ###  CTA :: Time stuff
  Serial.println(dateStr);
  String theTime = "";
  //theTime = time_t makeTime(byte sec, byte min, byte hour, byte day, byte month, int year );
  Serial.println(now());
}

// NPT server time retrieve code -------------------------------------------------------------------------------------------------------
// Found at (and slightly munged) http://www.esp8266.com/viewtopic.php?p=18395 posted by user "nigelbe", it is all the info I have, thank you Nigel
// Note that I've modified the dst function to (hopefully) get correct daylight savings time offset for USA
#define localTimeOffset 28800UL // your localtime offset from UCT in SECONDS :: PDT is 7 hours differnt than UTC - 
WiFiUDP udp;
unsigned int localPort = 2390; // local port to listen for UDP packets
const char* timeServer = "us.pool.ntp.org"; // fall back to regional time server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
bool setNTPtime() {
  time_t epoch = 0UL;
  if((epoch = getFromNTP(timeServer)) != 0){ // get from time server
    epoch -= 2208988800UL + localTimeOffset;
    setTime(epoch += dstUSA(epoch));
    nptGets++;
    return true;
  }
  return false;
}

unsigned long getFromNTP(const char* server) {
  udp.begin(localPort);
  sendNTPpacket(server); // send an NTP packet to a time server
  delay(1000); // wait to see if a reply is available
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
    return 0UL;
  }
  Serial.print("packet received, length=");
  Serial.println(cb);
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  udp.stop();
  return (unsigned long) highWord << 16 | lowWord;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(const char* server) {
  Serial.print("sending NTP packet to ");
  Serial.println(server);
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0; // Stratum, or type of clock
  packetBuffer[2] = 6; // Polling Interval
  packetBuffer[3] = 0xEC; // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(server, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

int dstUSA (time_t t) // calculate if summertime in USA (2nd Sunday in Mar, first Sunday in Nov)
{
  tmElements_t te;
  te.Year = year(t)-1970;
  te.Month = 3;
  te.Day = 1;
  te.Hour = 0;
  te.Minute = 0;
  te.Second = 0;
  time_t dstStart,dstEnd, current;
  dstStart = makeTime(te);
  dstStart = secondSunday(dstStart);
  dstStart += 2*SECS_PER_HOUR; //2AM
  te.Month=11;
  dstEnd = makeTime(te);
  dstEnd = firstSunday(dstEnd);
  dstEnd += SECS_PER_HOUR; //1AM
  if (t>=dstStart && t<dstEnd) {
    return (3600); //Add back in one hours worth of seconds - DST in effect
  } else {
    return (0); //NonDST
  }
}

time_t secondSunday(time_t t)
{
  t = nextSunday(t); //Once, first Sunday
  return nextSunday(t); // second Sunday
}

time_t firstSunday(time_t t)
{
  return nextSunday(t); //Once, first Sunday
}

////////////////////////////////////////////////////////
//
// PrintTime
//
// Serial Print time (DD/MM/YYYY - HH:MM:SS)
//
void PrintTime() {
  char cTime[22];
  
    sprintf(cTime, "%02u/%02u/%4u - %02u:%02u:%02u", day(), month(), year(), hour(), minute(), second());
    Serial.println(cTime);
}