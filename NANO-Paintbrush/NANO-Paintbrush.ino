/***************************************
  Chad Anderson
  Paintbrush 1.2
  Based on Arduino NANO
  Last Upload: 9/10/18 : 9:04
****************************************/


// #### LED SETUP #########################
#define interval 5000
#define FASTLED_INTERNAL
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ESP8266_RAW_PIN_ORDER
//#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER

#include <MyRealTimeClock.h>
#include "FastLED.h"
#include "Adafruit_NeoPixel.h"

MyRealTimeClock myRTC(13, 12, 11); // Assign Digital Pins 

#define NUM_LEDS1 1   //  7-6 = 1  Ring 1
#define NUM_LEDS2 7   //19-12 = 5  Ring 2
#define NUM_LEDS3 19  //37-18 = 19 Ring 3
#define NUM_LEDS4 37  //61-24 = 37 Ring 4
#define NUM_LEDS5 61  //155        Ring 5

#define NUM_LEDS_ALL 61 //155

#define PIN 7
#define NUMLEDS_1 = 1

uint8_t LED_Pin = 7;       // declare LED pin on NodeMCU Dev Kit

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
  Serial.begin(9600);

  // ##### RTC SETUP CODE BEGIN #######
  /* To set the current time and date in specific format 
  | Second 00 | Minute 59 | Hour 10 | Day 12 |  Month 07 | Year 2015 |
  */

  myRTC.setDS1302Time(00, 59, 10, 12 , 10, 07, 2015);

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


} //setup

// 02 - LOOP #################################################
void loop() {
  
  // #### RTC #############
  // Allow the update of variables for time / accessing the individual element. 
    myRTC.updateTime();
    Serial.print("Current Date / Time: "); 
    Serial.print(myRTC.dayofmonth); // Element 1
    Serial.print("/"); 
    Serial.print(myRTC.month); // Element 2
    Serial.print("/");
    Serial.print(myRTC.year); // Element 3
    Serial.print(" ");
    Serial.print(myRTC.hours); // Element 4
    Serial.print(":");
    Serial.print(myRTC.minutes); // Element 5
    Serial.print(":");
    Serial.println(myRTC.seconds); // Element 6
    delay( 5000);
  // #### END RTC #############


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
      rainbow();
      //droplets(1, 20,250, 0); // droplets( mode < 0=ring, 1=fill >, colorIncrement, speed, direction <0=out, 1=in> )
      //colorWipe(38,30,1); // colorWipe( colorIncrement, speed, direction <0=out, 1=in> )
      //colorflash( 0, 210, 1, 60, 50); //colorflash( mode <0,1,2>, color, colorIncrement, numFlashes <0-61>, speed);

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