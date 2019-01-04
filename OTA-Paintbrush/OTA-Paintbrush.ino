/***************************************
  Chad Anderson
  Paintbrush 1.3
  Based on Arduino OTA; HORAK NTP TIMER
  Last Upload: 9/16/18 : 17:30
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
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define INTERRUPT_THRESHOLD 1
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
//#define CLOCK_PIN 13

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
WiFi.setSleepMode(WIFI_NONE_SLEEP);

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

    // FastLED.addLeds<WS2811,7,RGB>(ring2, NUM_LEDS2).setCorrection(TypicalLEDStrip); //
    // FastLED.addLeds<WS2811,7,RGB>(ring3, NUM_LEDS3).setCorrection(TypicalLEDStrip); //
    // FastLED.addLeds<WS2811,7,RGB>(ring4, NUM_LEDS4).setCorrection(TypicalLEDStrip); //
    // FastLED.addLeds<WS2811,7,RGB>(ring5, NUM_LEDS5).setCorrection(TypicalLEDStrip); //
    FastLED.addLeds<WS2811,7,RGB>(ring6, NUM_LEDS_ALL).setCorrection(TypicalLEDStrip); //

    currentBlending = NOBLEND;
    fill_solid( ring6, NUM_LEDS_ALL, CHSV(0,0,0));
    //fill_solid( ring6, NUM_LEDS_ALL, CHSV(HUE_GREEN,255,255));

    for(int g=NUM_LEDS3; g <= NUM_LEDS4; g++){
      //fill_solid( ring4, NUM_LEDS4, CHSV(HUE_GREEN,255,255));
      ring6[g]=CHSV(gHue, 255, 255);
    }
    // #### FIRE WITH PALETTE SETUP ITEMS ##################
    //
    // This first palette is the basic 'black body radiation' colors,
    // which run from black to red to bright yellow to white.
      gPal = PartyColors_p;
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
            
      if ( ( getTimeSerial() > 1700 ) && ( getTimeSerial() < 2301 ) ) { // Run between 6pm (1800 ) and 10pm ( 2210 )
      
      //Default
      //rainbow();
      //droplets(0, 128, 0, 100, 1); // droplets( mode < 0=ring, 1=fill >, myHue, colorIncrement, speed, direction <0=out, 1=in> )
      //snowSparkle(145, 30, 80);

      if ( ( getTimeSerial() > 1700 ) && ( getTimeSerial() < 1800 ) ) { 
        snowSparkle(145, 60, 100);
      }
      if ( ( getTimeSerial() > 1801 ) && ( getTimeSerial() < 1900 ) ) { 
        snowSparkle(0, 30, 80);
      }
      if ( ( getTimeSerial() > 1901 ) && ( getTimeSerial() < 2000 ) ) { 
        snowSparkle(145, 100, 150);
      }
      if ( ( getTimeSerial() > 2001 ) && ( getTimeSerial() < 2100 ) ) { 
        snowSparkle(96, 30, 80);
      }
      if ( ( getTimeSerial() > 2101 ) && ( getTimeSerial() < 2200 ) ) { 
        snowSparkle(0, 80, 100);
      }
      if ( ( getTimeSerial() > 2201 ) && ( getTimeSerial() < 2301 ) ) { 
        snowSparkle(145, 30, 80);
      }
      // //  if ( (  minute() % 10 == 9 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 1900 ) && ( getTimeSerial() < 1905 ) ) { 
      //       snowSparkle(165, 30, 80);
      //     }

      //     if ( ( getTimeSerial() > 1910 ) && ( getTimeSerial() < 1920 ) ) { 
      //       juggle();
      //     }

      // //  if ( (  minute() % 10 == 8 ) ) { // Run on even hours only 
      //     if ( ( getTimeSerial() > 1930 ) && ( getTimeSerial() < 1935 ) ) { 
      //       droplets(1, 20,250, 0); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //     }
          
      //     if ( ( getTimeSerial() > 1945 ) && ( getTimeSerial() < 1950 ) ) { 
      //       cylon();
      //     }

      // //  if ( (  minute() % 10 == 7 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 2000 ) && ( getTimeSerial() < 2005 ) ) { 
      //       //colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       //colorflash( 0, 210, 1, 60, 50);
      //       theaterChaseRainbow(200);
      //     }

      //     if ( ( getTimeSerial() > 2010 ) && ( getTimeSerial() < 2020 ) ) { 
      //       colorWipe(5,20,0);
      //     }
      

      // //  if ( (  minute() % 10 == 6 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 2030 ) && ( getTimeSerial() < 2035 ) ) {  
      //       droplets(0, 20,50, 1); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //     }

      // //  if ( (  minute() % 10 == 5 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 2045 ) && ( getTimeSerial() < 2050 ) ) {  
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       confetti();
      //     }

      // //    if ( (  minute() % 10 == 4 ) ) { // Run on even hours only 
      //     if ( ( getTimeSerial() > 2100 ) && ( getTimeSerial() < 2105 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       meteorShower();
      //     }

      //     if ( ( getTimeSerial() > 2115 ) && ( getTimeSerial() < 2120 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       bpm();
      //     }


      //     if ( ( getTimeSerial() > 2130 ) && ( getTimeSerial() < 2135 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       Fire2012WithPalette();
      //     }

      // //   if ( (  minute() % 10 == 3 ) ) { // Run on even hours only 
      //     if ( ( getTimeSerial() > 2140 ) && ( getTimeSerial() < 2145 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       colorflash( 0, 210, 1, 60, 50);
      //     }

      // //  if ( (  minute() % 10 == 2 ) ) { // Run on even hours only 
      //     if ( ( getTimeSerial() > 2200 ) && ( getTimeSerial() < 2210 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       juggle();
      //     }

      // //  if ( (  minute() % 10 == 1 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 2230 ) && ( getTimeSerial() < 2240 ) ) {  
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //     }

      // //  if ( (  minute() % 10 == 0 ) ) { // Run on even hours only
      //     if ( ( getTimeSerial() > 2250 ) && ( getTimeSerial() < 2300 ) ) { 
      //       //colorWipe(5,20,0); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //       bpm();
      //     }



          
        FastLED.show();
       } else {
          FastLED.show();
          FastLED.clear();
      }
    
      

      // ### EFFECTS ####
      //fill_solid( leds7, NUM_LEDS_ALL, CHSV(HUE_RED,255,255));
      //rainbow();
      //droplets(1, 20,250, 0); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //colorflash( 0, 210, 1, 60, 50); //colorflash( mode <0,1,2>, color, colorIncrement, numFlashes <0-61>, speed);
      //cylon( );
      //sinelon( );
      //bpm();
      //confetti();
      //juggle();
      //meteorShower();
      //theaterChaseRainbow(200);
      //twinkle(210, 61, 50, 0); //twinkle( int myHue, int Count, int SpeedDelay, boolean OnlyOne )
      //snowSparkle(165, 30, 80); //snowSparkle(int myHue, int sparkleDelay, int speedDelay)
      //sinelonRings();
      //cylonRings(5);

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
// FADEALL // fadeall( )
//********************************************************************************
//
void fadeall() { for(int i = 0; i < NUM_LEDS_ALL; i++) { ring6[i].nscale8(150); } }

//********************************************************************************
// DROPLETS // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
//********************************************************************************
void droplets(int mode, int myHue, int colorIncrement, int speed, int direction) 
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
// CYLON // cylon( )
//********************************************************************************
void cylon(){
//static uint8_t hue = 0;
	//Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < NUM_LEDS_ALL; i++) {
		// Set the i'th led to red 
		ring6[i] = CHSV(gHue, 255, 255);
    ring6[i+1] = CHSV(gHue, 255, 255);
    ring6[i+2] = CHSV(gHue, 255, 255);
    ring6[i+3] = CHSV(gHue, 255, 255);

		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(40);
	}
	//Serial.print("x");

	// Now go in the other direction.  
	for(int i = (NUM_LEDS_ALL)-1; i >= 0; i--) {
		// Set the i'th led to red 
		ring6[i] = CHSV(gHue, 255, 255);
    ring6[i+1] = CHSV(gHue, 255, 255);
    ring6[i+2] = CHSV(gHue, 255, 255);
    ring6[i+3] = CHSV(gHue, 255, 255);

		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(40);
	}
}

//********************************************************************************
// CYLONRINGS // cylonRings( int ring <1-5> )
//********************************************************************************
void cylonRings(int ring){
//static uint8_t hue = 0;
	//Serial.print("x");
	// First slide the led in one direction
	for(int i = NUM_LEDS3; i < NUM_LEDS4; i++) {
		// Set the i'th led
		ring6[i] = CHSV(gHue, 255, 255);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		// Wait a little bit before we loop around and do it again
		delay(40);
    fadeall();
	}
	
  // for(int i = NUM_LEDS2; i < NUM_LEDS3; i++) {
	// 	ring6[i] = CHSV(gHue, 255, 255);
	// 	FastLED.show();
	// 	delay(40);
  //   fadeall();
	// }
  // for(int i = NUM_LEDS1; i < NUM_LEDS2; i++) {
	// 	ring6[i] = CHSV(gHue, 255, 255);
	// 	FastLED.show();
  //   delay(40);
  //   fadeall();
	// }


	// // Now go in the other direction.  
	// for(int i = (NUM_LEDS5)-1; i >= 0; i--) {
	// 	// Set the i'th led to red 
	// 	ring6[i] = CHSV(gHue, 255, 255);
  //   ring6[i+1] = CHSV(gHue, 255, 255);
  //   ring6[i+2] = CHSV(gHue, 255, 255);
  //   ring6[i+3] = CHSV(gHue, 255, 255);

	// 	// Show the leds
	// 	FastLED.show();
	// 	// now that we've shown the leds, reset the i'th led to black
	// 	// leds[i] = CRGB::Black;
	// 	fadeall();
	// 	// Wait a little bit before we loop around and do it again
	// 	delay(40);
	// }
}

//********************************************************************************
// TWINKLE // twinkle( int myHue, int Count, int SpeedDelay, boolean OnlyOne )
//********************************************************************************
void twinkle(int myHue, int Count, int SpeedDelay, boolean OnlyOne) {
  //setAll(0,0,0); 
  FastLED.clear();
  for (int i=0; i<Count; i++) {
     ring6[random(NUM_LEDS_ALL)]=CHSV(gHue, 255, 255);
     FastLED.show();
     delay(SpeedDelay);
     if(OnlyOne) { 
       //setAll(0,0,0); 
       FastLED.clear();
     }
   }
  delay(SpeedDelay);
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
// SNOWSPARKLE // snowSparkle( int myHue, int sparkleDelay, int speedDelay )
//********************************************************************************
void snowSparkle(int myHue, int sparkleDelay, int speedDelay) {
  //setAll(red,green,blue);
  if(myHue > 0){
    gHue = myHue;
  }
  fill_solid( ring6, NUM_LEDS_ALL, CHSV(gHue,255,255));
  int Pixel = random(NUM_LEDS_ALL);
  //setPixel(Pixel,0xff,0xff,0xff);
  ring6[random(NUM_LEDS_ALL)]=CRGB(255,255,255);
  ring6[random(NUM_LEDS_ALL)]=CRGB(255,255,255);
  ring6[random(NUM_LEDS_ALL)]=CRGB(255,255,255);
  ring6[random(NUM_LEDS_ALL)]=CRGB(255,255,255);

  FastLED.show();
  delay(sparkleDelay);
  ring6[Pixel]=CHSV(gHue,255,255);
  FastLED.show();
  delay(speedDelay);
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

//********************************************************************************
// THEATERCHASERAINBOW // theaterChaseRainbow( int delay )
//********************************************************************************
void theaterChaseRainbow(int SpeedDelay) {
  //byte *c;
  //for (int j=57; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < NUM_LEDS_ALL; i=i+3) {
          //c = Wheel( (i+j) % 255);
          //setPixel(i+q, *c, *(c+1), *(c+2));    //turn every third pixel on
          ring6[i+q] = CHSV(gHue,255,255);
        }
        FastLED.show();
        delay(SpeedDelay);
        for (int i=0; i < NUM_LEDS_ALL; i=i+3) {
          //setPixel(i+q, 0,0,0);        //turn every third pixel off
          ring6[i+q] = CHSV(0,0,0);
        }
    }
  //}
}

//********************************************************************************
// THEATERCHASE // theaterChase( )
//********************************************************************************
void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS_ALL; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      FastLED.show();
      delay(SpeedDelay);
      for (int i=0; i < NUM_LEDS_ALL; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}


//********************************************************************************
// RAINBOWWITHGLITTER // rainbowWithGlitter( )
//********************************************************************************
void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(90);
}
//********************************************************************************
// ADDGLITTER // addGlitter( )
//********************************************************************************
// ADD GLITTER TO EXISTING LOOP
  void addGlitter( uint8_t chanceOfGlitter) {
    if(random8() < chanceOfGlitter) {
      //leds[ random16(NUM_LEDS) ] += CRGB::White;
      //shortarmleds[ random16(SHORTNUM_LEDS) ] += CRGB::White;
      //longarmleds[ random16(LONGNUM_LEDS) ] += CRGB::White;
    }
    
  }
//********************************************************************************
// CONFETTI // confetti( )
//********************************************************************************
void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( ring6, NUM_LEDS_ALL, 10);
  int pos = random16(NUM_LEDS_ALL);
  ring6[pos] += CHSV( gHue + random8(64), 200, 255);
}

//********************************************************************************
// SINELON // sinelon( )
//********************************************************************************
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( ring6, NUM_LEDS_ALL, 80);
  int pos = beatsin16( 13, 0, NUM_LEDS_ALL-1 );
  ring6[pos] += CHSV( gHue, 255, 192);
}

//********************************************************************************
// SINELONRINGS // sinelonRings( )
//********************************************************************************
void sinelonRings()
{
  // a colored dot sweeping back and forth, with fading trails
  // fadeToBlackBy( ring2, NUM_LEDS2, 80);
  // fadeToBlackBy( ring3, NUM_LEDS3, 80);
  fadeToBlackBy( ring4, NUM_LEDS4, 80);
  // fadeToBlackBy( ring5, NUM_LEDS5, 80);

  // int pos2 = beatsin16( 13, 0, NUM_LEDS2-1 );
  // ring2[pos2] += CHSV( gHue, 255, 192);
  
  // int pos3 = beatsin16( 13, 0, NUM_LEDS3-1 );
  // ring3[pos3] += CHSV( gHue, 255, 192);

  int pos4 = beatsin16( 13, 0, NUM_LEDS4-1 );
  ring4[pos4] += CHSV( gHue, 255, 192);

  // int pos5 = beatsin16( 13, 0, NUM_LEDS5-1 );
  // ring5[pos5] += CHSV( gHue, 255, 192);


}

//********************************************************************************
// BOM // bpm( )
//********************************************************************************
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS_ALL; i++) { //9948
    ring6[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

//********************************************************************************
// JUGGLE // juggle( )
//********************************************************************************
void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( ring6, NUM_LEDS_ALL, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    ring6[beatsin16( i+7, 0, NUM_LEDS_ALL-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

////////////////////////////////////////////////////////////////////
// METEOR SHOWER 
// 5/27/2018
// Fast zooming meteors
// meteorShower()
////////////////////////////////////////////////////////////////////

uint8_t Shorthue = 50;
byte Shortidex = 255;
byte meteorLengthShort = 12;
int ShortbyteCount = 155;
byte Shorttemp[3];
  
void meteorShower(){

    // start by fading to black
    //fadeToBlackBy( leds, NUM_LEDS, 20);
   //for(int i=0; i<100; i++){
  // slide all the pixels down one in the array

  memmove8( &ring6[1], &ring6[0], ((NUM_LEDS_ALL - 1) * 3) );
 
  // increment the meteor display frame
  Shortidex++;
  // make sure we don't drift into space
  if ( Shortidex > meteorLengthShort ) {
    Shortidex = 255;
    // cycle through hues in each successive meteor tail
    Shorthue+=3;
    //hue = 90;  
  }

  // this switch controls the actual meteor animation, i.e., what gets placed in the
  // first position and then subsequently gets moved down the strip by the memmove above
  switch ( Shortidex ) {
  case 0:
    ring6[0] = CRGB(200,200,200); 
    break;
  case 1:
    ring6[0] = CHSV((Shorthue), 255, 210);
    break;
  case 2:
    ring6[0] = CHSV((Shorthue), 255, 200); 
    break;
  case 3:
    ring6[0] = CHSV((Shorthue), 255, 190); 
    break;
  case 4:
    ring6[0] = CHSV((Shorthue), 255, 180); 
    break;
  case 5:
    ring6[0] = CHSV((Shorthue), 255, 170); 
    break;
  case 6:
    ring6[0] = CHSV((Shorthue), 160, 160);
    break;
  case 7:
    ring6[0] = CHSV((Shorthue), 140, 170);
    break;
  case 8:
    ring6[0] = CHSV((Shorthue), 120, 160);  
    break;
  case 9:
    ring6[0] = CHSV((Shorthue), 100, 150);
    break;
  case 10:
    ring6[0] = CHSV((Shorthue), 100, 140);
    break;
  case 11:
    ring6[0] = CHSV((Shorthue), 100, 150);
    break;
  case 12:
    ring6[0] = CHSV((Shorthue ), 100, 150);
    break;
  
  default:
    ring6[0] = CRGB::Black; 
  }

  // show the blinky
  FastLED.show();  
  // control the animation speed/frame rate
  delay(20);
     //}
  
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

// ##### HELPER FUNCTIONS #####
byte * Wheel(byte WheelPos) {
  static byte c[3];
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }
  return c;
}

//********************************************************************************
// setPixel
//********************************************************************************
void setPixel(int Pixel, byte red, byte green, byte blue) {
  ring6[Pixel].r = red;
  ring6[Pixel].g = green;
  ring6[Pixel].b = blue;
}

//********************************************************************************
// setAll
//********************************************************************************
void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS_ALL; i++ ) {
    setPixel(i, red, green, blue); 
  }
  FastLED.show(); 
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