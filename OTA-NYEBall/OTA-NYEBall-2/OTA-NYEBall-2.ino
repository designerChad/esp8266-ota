/***************************************
  Chad Anderson
  Cactus 1.0
  Based on Arduino OTA; HORAK NTP TIMER
  Last Upload: 9/15/18 : 15:47
****************************************/

// Both of these are included when you pick the Time library in Arduino IDE library manager
//https://github.com/michaelmargolis/arduino_time/tree/master/Time

#include <Time.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h> //horack
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "/Users/chad/documents/arduino/includes/netconn" //2011
//#include "/Users/chadwickanderson/documents/arduino/includes/netconn" //2017

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
// /WEBSERVER SETUP #######################################



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
#define FASTLED_INTERNAL
#include "FastLED.h"
//#define NUM_LEDS 22 //155

#define NUM_LEDS 300 //155
#define SHORTNUM_LEDS 70 //155
#define LONGNUM_LEDS 95 //155

#define NUM_STRIPS 3
//#define CLK_PIN   13

struct CRGB leds[NUM_LEDS];
struct CRGB shortarmleds[SHORTNUM_LEDS];
struct CRGB longarmleds[LONGNUM_LEDS];

uint8_t gBrightness = 180;
#define UPDATES_PER_SECOND 100
#define FRAMES_PER_SECOND  120

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
TBlendType    currentBlending;

int myBrightness = 0 ;
int afterMidnight = 2 ; // 1 = yes; 0 = no; 2 = force countdown ; 3 = mySetup
int hourOffset = 15 ;
// 01 - SETUP ########################################

void setup() {

  // clear all LEDs
  FastLED.clear();

    fill_solid( leds, 300, CRGB(0,0,0)); //120max
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 270, CRGB(255,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 150, CHSV(HUE_RED,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(0,0,0)); //120max
    FastLED.show();
    delay(5000);

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
   ArduinoOTA.setHostname("NYEBall");

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

// #####  / OTA ESSENTIALS ##########


// ##### LED SETUP CODE BEGIN #######

  FastLED.addLeds<WS2812B,5,RGB>(shortarmleds, SHORTNUM_LEDS).setCorrection(TypicalLEDStrip); // SHORT ARM
  FastLED.addLeds<WS2811,6,RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // BODY
  FastLED.addLeds<WS2812B,7,RGB>(longarmleds, LONGNUM_LEDS).setCorrection(TypicalLEDStrip); // LONG ARM
  currentBlending = NOBLEND;
  

  // #### WEBSERVER STARTUP
  server.begin();

} //setup

// 02 - LOOP #################################################
void loop() {
  ArduinoOTA.handle(); // startup OTA handle
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
              client.println("<body><h1>Cactus 1.0 Web Server</h1>");
              
             

                client.println("<p>Cactus 1.0</p>");
                client.println("<p>");
                  client.println( year() );
                client.println("</p>");
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
                client.println("<p>TimeSerial Fn :: "); 
                  client.println( getTimeSerial ()); 
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

  // #################################################
  //
  //  LED LOOP
  //
  // #################################################

    // SETUP SHORT ARM COLORS - PIN 5 :: ttl 70 (arm is 20)
      // fill_solid( shortarmleds, SHORTNUM_LEDS, CHSV(HUE_ORANGE,255,255));
      // fill_solid( shortarmleds, 66, CHSV(HUE_GREEN,255,255));
      // fill_solid( shortarmleds, 35, CHSV(HUE_BLUE,255,255));

    // SETUP BODY COLORS - PIN 6
      // fill_solid( leds, NUM_LEDS, CHSV(HUE_RED,255,255));
      // fill_solid( leds, 127, CHSV(HUE_ORANGE,255,255));
      // fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      // fill_solid( leds, 78, CHSV(HUE_GREEN,255,255));      
      // fill_solid( leds, 55, CHSV(HUE_BLUE,255,255));
      // fill_solid( leds, 30, CHSV(HUE_PURPLE,255,255));

    // SETUP LONG ARM COLORS - PIN 7 :: ttl 98 (arm is 20)
      // fill_solid( longarmleds, LONGNUM_LEDS, CHSV(HUE_RED,255,255));
      // fill_solid( longarmleds, 92, CHSV(HUE_ORANGE,255,255));
      // fill_solid( longarmleds, 66, CHSV(HUE_GREEN,255,255));
      // fill_solid( longarmleds, 35, CHSV(HUE_BLUE,255,255));
      
      //EVERY_N_MILLISECONDS( 100 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      
      // insert a delay to keep the framerate modest
      //FastLED.delay(1000/FRAMES_PER_SECOND);

      //addGlitter(100);
      
      //rainbowWithGlitter();


      //#####################
      //  TIMER
      //#####################
      //if ( ( hour() > 16 ) && ( hour() <= 22 ) ) { 
      //if ( ( getTimeSerial() > 900 ) && ( getTimeSerial() < 2359 ) ) { // Run between 6pm (1800 ) and 10pm ( 2210 )
      //if ( ( hour() % 2 == 0 ) ) { // Run on even hours only 

         // FastLED.show();
      //} else {
          // fill_solid( leds, NUM_LEDS, CHSV(0,0,0));
          // fill_solid( longarmleds, NUM_LEDS, CHSV(0,0,0));
          // fill_solid( shortarmleds, NUM_LEDS, CHSV(0,0,0));
          // FastLED.show();
          // FastLED.clear();
      //}

//Serial.println( t.min );
//Time myTime = rtc.time();
//int myMinute;
//myMinute = printMinute();
//t.yr, t.mon, t.date, t.hr, t.min, t.sec);
//12 = 0
//11 = 23
//10 = 22
//9 = 21
//8 = 20
//7 = 19
//6 = 18
//5 = 17
//4 = 16
//3 = 15
//Serial.println(hour());

// EVERY_N_MILLISECONDS(1000) {
//   myBrightness += 4;
// }

// if (second() == 0 ){
//   myBrightness = 15;
// }else{
//   //maybe do every 15 minutes (4 increments of 60 + 15)
//   if ( second() <= 15 ){
//     myBrightness =  75;
//   } else if ( second() > 15 && second() <=30 ) {
//     myBrightness =  135;
//   } else if ( second() > 30 && second() <=45 ) {
//     myBrightness =  195;
//   } else if ( second() >= 55 ){
//     myBrightness = 255;
//   }
// }

// RUN SETUP CHECK
//mySetup();

if (minute() == 0 ){
  myBrightness = 85;
}else{
  //maybe do every 15 minutes (4 increments of 60 + 15)
  if ( minute() <= 15 ){
    myBrightness =  85;
  } else if ( minute() > 15 && minute() < 30 ) {
    myBrightness =  170;
  } else if ( minute() > 45 ) {
    myBrightness =  255;
  }
}

if ( afterMidnight == 0 ){ //before midnight
    //Serial.println(myBrightness);
    //Serial.println(second());
    if( hour() == ( 18 ) || hour() == ( 12 ) ){ //6pm
    //if( minute() == 10 ){
      //Serial.println(myTime.hr);
      // increase brightness by 5 each second
      //myBrightness += second() + 5;
      
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,myBrightness));
    }

    if( hour() == ( 19 ) || hour() == ( 13 ) ){
      //if( minute() == 11 ){
      //Serial.println(myTime.hr);
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,myBrightness));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    }

    if( hour() == ( 20 ) || hour() == ( 14 ) ){
      //if( minute() == 12 ){
      //Serial.println(myTime.hr);
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,myBrightness));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    }

    if( hour() == ( 21 ) || hour() == ( 15 ) ){
    //  if( minute() == 13 ){
      //Serial.println(myTime.hr);
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,myBrightness));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      
    }

    if( hour() == ( 22 ) || hour() == ( 16 ) ){
    // if( minute() == 14 ){
      //Serial.println(myTime.hr);
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 125, CHSV(HUE_ORANGE,255,myBrightness));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    }

    if( hour() == ( 23 ) || hour() == ( 17 ) ){ // 20 = 8pm
      //if( minute() == 15 ){
      //Serial.println(myTime.hr);
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 150, CHSV(HUE_RED,255,myBrightness));      
      fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));

      if ( minute() >= 57 && minute() < 59 ) { // 2 minutes
        FastLED.show();
        delay(500);
        fill_solid( leds, 300, CRGB(0,0,0));
        FastLED.show();
        delay(500);
      }

      if ( minute() == 59 ){
        fill_solid( leds, 300, CRGB(0,0,0));
        addGlitter(150);
        FastLED.show();
        if (second() >= 54 ){
          myCountdown();
        }
      }


    }

} //afterMidnight == 0
else if ( afterMidnight == 2 ) { // force countdown
  myCountdown();
// } else if (afterMidnight == 3 ){
//   mySetup();
}
else  { // afterMidnight == 1

  //if( hour() == 21 ){
    //if( minute() == 16 ){
    //Serial.println(myTime.hr);
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 300, CRGB(255,255,255));
    fill_solid( leds, 150, CHSV(HUE_RED,255,255));
    fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
    fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
    fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
    fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
    fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
  
    addGlitter(400);
    //FastLED.show();
    //delay(10000);

    //afterMidnight = 0 ;
 // }
} //afterMidnight == 1

FastLED.show();


} // LOOP

// #################################################
//
//  FUNCTIONS
//
// #################################################


void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  fill_rainbow( shortarmleds, NUM_LEDS, gHue, 7);
  fill_rainbow( longarmleds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(90);
}

// ADD GLITTER TO EXISTING LOOP
  void addGlitter( uint8_t chanceOfGlitter) {
    if(random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
      //shortarmleds[ random16(SHORTNUM_LEDS) ] += CRGB::White;
      //longarmleds[ random16(LONGNUM_LEDS) ] += CRGB::White;
    }
    
  }
// Setup ROUTINE
  void mySetup( ) {
    fill_solid( leds, 300, CRGB(0,0,0));
    fill_solid( leds, 300, CRGB(255,255,255));
    fill_solid( leds, 150, CHSV(HUE_RED,255,255));
    fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
    fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
    fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
    fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
    fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(255,255,255));
    fill_solid( leds, 150, CHSV(HUE_RED,255,255));
    fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
    fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
    fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
    fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
    fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    FastLED.show();
    delay(500);
    fill_solid( leds, 300, CRGB(255,255,255));
    fill_solid( leds, 150, CHSV(HUE_RED,255,255));
    fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
    fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
    fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
    fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
    fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
    FastLED.show();
    delay(500);

    //afterMidnight = 0;
  }



// FINAL COUNTDOWN ROUTINE
  void myCountdown( ) {
    //delay();
    //leds[ random16(NUM_LEDS) ] += CRGB::White;

    fill_solid( leds, 300, CRGB(0,0,0));

    FastLED.show();
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show(); //6

     delay(1000); 
    
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show();//5

    delay(1000);

      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show();//4

    delay(1000);

      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show();//3

    delay(1000);
    
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show();//2

    delay(1000);
    
      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 150, CHSV(HUE_RED,255,255));
      fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));
      FastLED.show();//1

    delay(1000);

      fill_solid( leds, 300, CRGB(0,0,0));
      fill_solid( leds, 270, CRGB(255,255,255));
      fill_solid( leds, 150, CHSV(HUE_RED,255,255));
      fill_solid( leds, 125, CHSV(HUE_ORANGE,255,255));
      fill_solid( leds, 100, CHSV(HUE_YELLOW,255,255));
      fill_solid( leds, 75, CHSV(HUE_GREEN,255,255));
      fill_solid( leds, 50, CHSV(HUE_BLUE,255,255));
      fill_solid( leds, 25, CHSV(HUE_PURPLE,255,255));

    FastLED.show(); // 0 Happy new year!!!

    afterMidnight = 1;
    //delay();
  }

// ##### MY TIME FUNCTIONS #####
int getTimeSerial () {
  int theHour = hour();
  int theMinute = minute();
  int myTime = ( (theHour * 100) + theMinute );
  return myTime;
}







//********************************************************************************
// TIME SERVER ETC
//********************************************************************************

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
  //Serial.println(dateStr);
  String theTime = "";
  //theTime = time_t makeTime(byte sec, byte min, byte hour, byte day, byte month, int year );
  //Serial.println(now());
  Serial.println( hour() );
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