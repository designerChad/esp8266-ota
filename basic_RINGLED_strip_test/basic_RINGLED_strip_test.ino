 // Arduino Relay Control Code
 
#define interval 5000

#include "FastLED.h"
#include "Adafruit_NeoPixel.h"

#define NUM_LEDS1 1 //155
#define NUM_LEDS2 1 //7-6
#define NUM_LEDS3 7 //19-12 = 5
#define NUM_LEDS4 19 //37-18 = 19
#define NUM_LEDS5 37 //61-24 = 
#define NUM_LEDS6 61 //155

#define PIN 7
#define NUMLEDS_1 = 1
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(16, PIN);
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS_1, PIN, RGB);     //AdaFruit Neo Library


struct CRGB leds3[NUM_LEDS1];
struct CRGB leds4[NUM_LEDS2];
struct CRGB leds5[NUM_LEDS3];
struct CRGB leds6[NUM_LEDS4];
struct CRGB leds7[NUM_LEDS6];

uint8_t gBrightness = 30;
#define UPDATES_PER_SECOND 100


void setup() {
    FastLED.addLeds<WS2811,3,RGB>(leds3, NUM_LEDS1).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2811,4,RGB>(leds4, NUM_LEDS2).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2811,5,RGB>(leds5, NUM_LEDS3).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2811,6,RGB>(leds6, NUM_LEDS4).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2811,7,RGB>(leds7, NUM_LEDS6).setCorrection(TypicalLEDStrip); // TALL ARM



}
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds7, NUM_LEDS6, gHue, 1);
}

void loop()
{

  
//  fill_solid( leds3, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds4, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds5, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds6, NUM_LEDS, CHSV(0,0,0));
//  fill_solid( leds7, NUM_LEDS, CHSV(0,0,0));

//  FastLED.show();

//
//     delay(2000);
//  fill_solid( leds7, NUM_LEDS6,CHSV(HUE_BLUE,255,255));
//  fill_solid( leds7, NUM_LEDS5,CHSV(HUE_RED,255,255));
//  fill_solid( leds7, NUM_LEDS4,CHSV(HUE_GREEN,255,255));
//  fill_solid( leds7, NUM_LEDS3,CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds7, NUM_LEDS2,CHSV(HUE_ORANGE,255,255));

//fill_solid(leds7[0],1,CHSV(HUE_BLUE,255,255));
  
//        FastLED.show();
//   delay(2000);
//  fill_solid( leds3, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds4, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds5, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds6, NUM_LEDS, CHSV(HUE_PURPLE,255,255));
//
//        FastLED.show();
//   delay(2000);
        

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

    rainbow();
    EVERY_N_MILLISECONDS( 100 ) { gHue--; }
  FastLED.show();
   
}

