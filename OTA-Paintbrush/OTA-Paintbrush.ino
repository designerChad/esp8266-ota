/***************************************
  Chad Anderson
  Paintbrush 1.2
  Based on Arduino OTA; HORAK NTP TIMER
  Last Upload: 9/10/18 : 9:04
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

//#include "/Users/chad/documents/arduino/includes/netconn" //2011
#include "/Users/chadwickanderson/documents/arduino/includes/netconn" //2017

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
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER

#include "FastLED.h"
#include "Adafruit_NeoPixel.h"

#define NUM_LEDS1 1   //  7-6 = 1  Ring 1
#define NUM_LEDS2 7   //19-12 = 5  Ring 2
#define NUM_LEDS3 19  //37-18 = 19 Ring 3
#define NUM_LEDS4 37  //61-24 = 37 Ring 4
#define NUM_LEDS5 61  //155        Ring 5

#define NUM_LEDS_ALL 61 //155

#define PIN D7
#define NUMLEDS_1 = 1

uint8_t LED_Pin = D7;       // declare LED pin on NodeMCU Dev Kit

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(16, PIN);
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS_1, PIN, RGB);     //AdaFruit Neo Library

struct CRGB ring1[NUM_LEDS1];
struct CRGB ring2[NUM_LEDS2];
struct CRGB ring3[NUM_LEDS3];
struct CRGB ring4[NUM_LEDS4];
struct CRGB ring5[NUM_LEDS5];
struct CRGB ring6[NUM_LEDS_ALL];

uint8_t gBrightness = 180;
#define UPDATES_PER_SECOND 100
#define FRAMES_PER_SECOND  120

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t myHue = 0;
TBlendType    currentBlending;
CRGBPalette16 gPal;
bool gReverseDirection = false;
uint8_t myCnt = 1; //counter for ring droplet();

// 01 - SETUP ########################################

void setup() {

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
   ArduinoOTA.setHostname("ChadzPaintbrush2");

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
  //pinMode(ESP_BUILTIN_LED, OUTPUT);
  pinMode(LED_Pin, OUTPUT);   // Initialize the LED pin as an output

// #####  / OTA ESSENTIALS ##########


// ##### LED SETUP CODE BEGIN #######

// clear all LEDs
    FastLED.clear();

    //FastLED.addLeds<WS2811,3,RGB>(leds3, NUM_LEDS1).setCorrection(TypicalLEDStrip); //
    //FastLED.addLeds<WS2811,4,RGB>(leds4, NUM_LEDS2).setCorrection(TypicalLEDStrip); //
    //FastLED.addLeds<WS2811,5,RGB>(leds5, NUM_LEDS3).setCorrection(TypicalLEDStrip); //
    //FastLED.addLeds<WS2811,6,RGB>(leds6, NUM_LEDS_ALL).setCorrection(TypicalLEDStrip); //
    FastLED.addLeds<WS2811,7,RGB>(ring6, NUM_LEDS_ALL).setCorrection(TypicalLEDStrip); //

    currentBlending = NOBLEND;
    fill_solid( ring6, NUM_LEDS_ALL, CHSV(0,0,0));
    fill_solid( ring6, NUM_LEDS_ALL, CHSV(HUE_GREEN,255,255));

   
    // #### FIRE WITH PALETTE SETUP ITEMS ##################
    //
    // This first palette is the basic 'black body radiation' colors,
    // which run from black to red to bright yellow to white.
      gPal = HeatColors_p;
      //gPal = ForestColors_p;
    //   
    // These are other ways to set up the color palette for the 'fire'.
    // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
    //    
    // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
    // 
    // Third, here's a simpler, three-step gradient, from black to red to white
    //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
    // ######################################################


    FastLED.show();

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
              client.println("<body><h1>Paintbrush 1.0 Web Server</h1>");
              
             

                client.println("<p>Paintbrush 1.0</p>");
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
                client.println("<p>MyCnt :: "); 
                  client.println( myCnt ); 
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
    // Add entropy to random number generator; we use a lot of it.
    //random16_add_entropy( rand());
    myCnt+=1; // used for ring cycle counting // myCnt % 5
    EVERY_N_MILLISECONDS( 300 ) { gHue--; } // slowly cycle the "base color" through the rainbow
      
    // insert a delay to keep the framerate modest
    //FastLED.delay(1000/FRAMES_PER_SECOND);


      //#####################
      //  TIMER
      //#####################
      //if ( ( hour() > 16 ) && ( hour() <= 22 ) ) { 
      
      // if ( ( getTimeSerial() > 1100 ) && ( getTimeSerial() < 2200 ) ) { // Run between 6pm (1800 ) and 10pm ( 2210 )
      
      //   if ( (  minute() % 5 == 4 ) ) { // Run on even hours only 
      //     rainbow();
      //   }
      //   if ( (  minute() % 5 == 3 ) ) { // Run on even hours only 
      //     droplets(1, 20,250, 0); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //   }
      //   if ( (  minute() % 5 == 2 ) ) { // Run on even hours only 
      //     colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //   }
      //   if ( (  minute() % 5 == 1 ) ) { // Run on even hours only 
      //     droplets(0, 20,50, 1); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //   }
      //   if ( (  minute() % 5 == 0 ) ) { // Run on even hours only 
      //     colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //   }

      //   FastLED.show();
      //  } else {
      //     FastLED.show();
      //     FastLED.clear();
      // }
    
      // ### EFFECTS ####
      //fill_solid( leds7, NUM_LEDS_ALL, CHSV(HUE_RED,255,255));
      //rainbow();
      //droplets(1, 20,250, 0); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      colorflash( 0, 210, 1, 60, 50); //colorflash( mode <0,1,2>, color, colorIncrement, numFlashes <0-61>, speed);

      // ### FIRE WITH PALETTE
      // Fourth, the most sophisticated: this one sets up a new palette every
      // time through the loop, based on a hue that changes every time.
      // The palette is a gradient from black, to a dark color based on the hue,
      // to a light color based on the hue, to white.
      //
      //   static uint8_t hue = 0;
      //   hue++;
      //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
      //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
      //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
      //Fire2012WithPalette(); // run simulation frame, using palette colors



    FastLED.show();
} // LOOP

// #####################################################################################
//
//  FUNCTIONS
//
// #####################################################################################


//********************************************************************************
// RAINBOW // rainbow( )
//********************************************************************************
// rainbow color cycle
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( ring6, NUM_LEDS_ALL, gHue, 1);
}


//********************************************************************************
// DROPLETS // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
//********************************************************************************
void droplets(int mode, int colorIncrement, int speed, int direction) 
{
  //Serial.println(myCnt);
  //Serial.println(myCnt % 5);
  // fill_solid( ring6, NUM_LEDS5, CHSV(HUE_YELLOW,255,255));
  //switch (myCnt % 5)
  // 1 % 5 = 1
  // 2 % 5 = 2
  // 3 % 5 = 3
  // 4 % 5 = 4
  // 5 % 5 = 0
  if(mode == 0){ // clear for droplet rings
    FastLED.clear(); 
  }else{ // clear every 5 cycles to fill rings
    if ( myCnt % 5 == 1 ){
      FastLED.clear();
    }
  }

  if (direction == 0){ //INWARD TO OUTWARD DIRECTION
    if ( myCnt % 5 == 1){
      for(int z = 0  ; z < 1  ; z++){
          ring6[z] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 2){
      for(int a = 1  ; a < 7  ; a++){
          ring6[a] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 3){
      for(int b = 7  ; b < 19  ; b++){
          ring6[b] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 4){
      for(int c = 19 ; c < 37 ; c++){
          ring6[c] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 0){
      for(int d = 37 ; d < 61 ; d++){
          ring6[d] = CHSV (myHue, 255, 255);
      }
    }
  }else{ // OTWARD TO INWARD DIRECTION
    if ( myCnt % 5 == 0){
      for(int z = 0  ; z < 1  ; z++){
          ring6[z] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 4){
      for(int a = 1  ; a < 7  ; a++){
          ring6[a] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 3){
      for(int b = 7  ; b < 19  ; b++){
          ring6[b] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 2){
      for(int c = 19 ; c < 37 ; c++){
          ring6[c] = CHSV (myHue, 255, 255);
      }
    }
    if ( myCnt % 5 == 1){
      for(int d = 37 ; d < 61 ; d++){
          ring6[d] = CHSV (myHue, 255, 255);
      }
    }
  }

  delay(speed);
  if (myCnt % 5 == 0){ // increment color ever 5 cycles
    myHue+=colorIncrement;
  }
}

//********************************************************************************
// COLORWIPE // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
//********************************************************************************
void colorWipe(int colorIncrement, int speed, int direction){
  for(int i=0; i<NUM_LEDS_ALL; i++){
    if(direction == 0){
      ring6[i] = CHSV(myHue, 255, 255);
    }
    else{
      ring6[NUM_LEDS_ALL-1-i] = CHSV(myHue, 255, 255);
    }
    FastLED.show();
    delay(speed);
  }
  myHue+=colorIncrement;
  Serial.println(myHue);
}

//********************************************************************************
// COLORFLASH // colorflash( mode, color, colorIncrement, numFlashes <0-61>, speed)
//********************************************************************************
void colorflash(int mode, int color, int colorIncrement, int numFlashes, int speed){
  int flashes[numFlashes];
  FastLED.clear(); 

  //mode 0=, 1=randomflash, 2=full flash

  if (colorIncrement > 0) {
    myHue+=colorIncrement;
  }else{
    myHue=color;
  }

  //for(int i=0; i<cycles; i++){
    for(int g=0; g<numFlashes; g++){
      int idx = random(NUM_LEDS_ALL);
      if (numFlashes == NUM_LEDS_ALL){
        flashes[g] = idx;
        ring6[g] = CHSV(myHue,255,255);
      }else{
        flashes[g] = idx;
        ring6[idx] = CHSV(myHue,255,255);
      }
    }
    FastLED.show();
    delay(speed);
    for(int s=0; s<numFlashes; s++){
      ring6[flashes[s]] = CHSV(0,0,0);
    }
    delay(speed);

   
  //}
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
      //leds[ random16(NUM_LEDS) ] += CRGB::White;
      //shortarmleds[ random16(SHORTNUM_LEDS) ] += CRGB::White;
      //longarmleds[ random16(LONGNUM_LEDS) ] += CRGB::White;
    }
    
  }


//********************************************************************************
// FIRE WITH PALETTE
//********************************************************************************

// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 50


void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS_ALL];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS_ALL; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS_ALL) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS_ALL - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS_ALL; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS_ALL-1) - j;
      } else {
        pixelnumber = j;
      }
      ring6[pixelnumber] = color;
    }
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