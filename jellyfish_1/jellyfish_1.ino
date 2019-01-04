// NANO - ATMega328P (old bootloader)
 
#include "FastLED.h"
#define NUM_LEDSshort 12 //short
#define NUM_LEDSlong 21 //long

struct CRGB leds3[NUM_LEDSlong]; // 2 LONG
struct CRGB leds4[NUM_LEDSshort]; // 3 SHORT
struct CRGB leds5[NUM_LEDSlong];
struct CRGB leds6[NUM_LEDSshort];

//Lightning constants
#define FREQUENCY     30                // controls the interval between strikes
#define FLASHES       15                 // the upper limit of flashes per strike
#define BRIGHTNESS    255

//CRGB leds[NUM_LEDS];
#define color White;
unsigned int dimmer = 1;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t myHue = 0;
uint8_t gBrightness = 180;
#define UPDATES_PER_SECOND 50


void setup() {

    delay(3000); // allows reprogramming if accidently blowing power w/leds
    LEDS.setBrightness(BRIGHTNESS);

    FastLED.addLeds<WS2812B,3,GRB>(leds3, NUM_LEDSlong).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,4,GRB>(leds4, NUM_LEDSshort).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,5,GRB>(leds5, NUM_LEDSlong).setCorrection(TypicalLEDStrip); // TALL ARM
    FastLED.addLeds<WS2812B,6,GRB>(leds6, NUM_LEDSshort).setCorrection(TypicalLEDStrip); // TALL ARM

  

}
void loop()
{
//  fill_solid( leds3, NUM_LEDSlong, CHSV(0,0,0));
//  fill_solid( leds4, NUM_LEDSshort, CHSV(0,0,0));
//  fill_solid( leds5, NUM_LEDSlong, CHSV(0,0,0));
//  fill_solid( leds6, NUM_LEDSshort, CHSV(0,0,0));

//  FastLED.show();
//
//
//   delay(150);
//  fill_solid( leds3, NUM_LEDSlong,CHSV(HUE_BLUE,255,255));
//  fill_solid( leds4, NUM_LEDSshort,CHSV(HUE_RED,255,255));
//  fill_solid( leds5, NUM_LEDSlong,CHSV(HUE_YELLOW,255,255));
//  fill_solid( leds6, NUM_LEDSshort,CHSV(HUE_PURPLE,255,255));
//  
//        FastLED.show();
//   delay(150);
//  fill_solid( leds3, NUM_LEDSlong, CHSV(HUE_PURPLE,255,255));
//  fill_solid( leds4, NUM_LEDSshort, CHSV(HUE_RED,255,255));
//  fill_solid( leds5, NUM_LEDSlong, CHSV(HUE_GREEN,255,255));
//  fill_solid( leds6, NUM_LEDSshort, CHSV(HUE_YELLOW,255,255));
//
//        FastLED.show();
//   delay(150);
//       
//        fill_solid( leds3, NUM_LEDSlong, CHSV(HUE_RED,255,255));
//        fill_solid( leds4, NUM_LEDSshort, CHSV(HUE_PURPLE,255,255));
//        fill_solid( leds5, NUM_LEDSlong, CHSV(HUE_YELLOW,255,255));
//        fill_solid( leds6, NUM_LEDSshort, CHSV(HUE_ORANGE,255,255));
//
//        FastLED.show();
//   delay(150);
//
//         fill_solid( leds3, NUM_LEDSlong, CHSV(HUE_GREEN,255,255));
//         fill_solid( leds4, NUM_LEDSshort, CHSV(HUE_ORANGE,255,255));
//         fill_solid( leds5, NUM_LEDSlong, CHSV(HUE_RED,255,255));
//         fill_solid( leds6, NUM_LEDSshort, CHSV(HUE_BLUE,255,255));
//meteorShowerLong(); 
//meteorShowerShort();
snowSparkleLong(85, 30, 50); //170  good blue
snowSparkleShort(255, 30, 50);
//lightningLong();
//lightningShort();

         FastLED.show();
}


//********************************************************************************
// SNOWSPARKLE // snowSparkle( int myHue, int sparkleDelay, int speedDelay )
//********************************************************************************
void snowSparkleLong(int myHue, int sparkleDelay, int speedDelay) {
  //setAll(red,green,blue);
  if(myHue > 0){
    gHue = myHue;
  }
  fill_solid( leds3, NUM_LEDSlong, CHSV(gHue,255,200));
   fill_solid( leds5, NUM_LEDSlong, CHSV(gHue,255,200));
  int Pixel = random(NUM_LEDSlong);
  //setPixel(Pixel,0xff,0xff,0xff);
  leds3[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds3[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds3[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds3[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds5[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds5[random(NUM_LEDSlong)]=CRGB(255,255,255);
  // leds5[random(NUM_LEDSlong)]=CRGB(255,255,255);
  leds5[random(NUM_LEDSlong)]=CRGB(255,255,255);

  FastLED.show();
  delay(sparkleDelay);
  leds3[Pixel]=CHSV(gHue,255,255);
  leds5[Pixel]=CHSV(gHue,255,255);
  FastLED.show();
  delay(speedDelay);
}
//********************************************************************************
// SNOWSPARKLE // snowSparkle( int myHue, int sparkleDelay, int speedDelay )
//********************************************************************************
void snowSparkleShort(int myHue, int sparkleDelay, int speedDelay) {
  //setAll(red,green,blue);
  if(myHue > 0){
    gHue = myHue;
  }
  fill_solid( leds4, NUM_LEDSshort, CHSV(gHue,255,200));
  fill_solid( leds6, NUM_LEDSshort, CHSV(gHue,255,200));
  int Pixel = random(NUM_LEDSshort);
  //setPixel(Pixel,0xff,0xff,0xff);
  leds4[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds4[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds4[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds4[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds6[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds6[random(NUM_LEDSshort)]=CRGB(255,255,255);
  // leds6[random(NUM_LEDSshort)]=CRGB(255,255,255);
  leds6[random(NUM_LEDSshort)]=CRGB(255,255,255);

  FastLED.show();
  delay(sparkleDelay);
  leds4[Pixel]=CHSV(gHue,255,255);
  leds6[Pixel]=CHSV(gHue,255,255);
  FastLED.show();
  delay(speedDelay);
}
/////////////////////////////////////////////
// LIGHTNING
// 10/6/2018
// Flashing lightning
/////////////////////////////////////////////
// The first "flash" in a bolt of lightning is the "leader." The leader
// is usually duller and has a longer delay until the next flash. Subsequent
// flashes, the "strokes," are brighter and happen at shorter intervals.

void lightningLong(){
  for (int flashCounter = 0; flashCounter < random8(3,FLASHES); flashCounter++)
    {
      if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
      else dimmer = random8(1,3);           // return strokes are brighter than the leader
      
      fill_solid(leds3,NUM_LEDSlong,CHSV(255, 0, 255/dimmer));
      fill_solid(leds5,NUM_LEDSlong,CHSV(255, 0, 255/dimmer));


      FastLED.show();
      delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
      fill_solid(leds3,NUM_LEDSlong,CHSV(0, 0, 0));
      fill_solid(leds5,NUM_LEDSlong,CHSV(0, 0, 0));

      FastLED.show();
      
      if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
      delay(50+random8(100));               // shorter delay between strokes  
    }
    delay(random8(FREQUENCY)*100);          // delay between strikes
}
void lightningShort(){
  for (int flashCounter = 0; flashCounter < random8(3,FLASHES); flashCounter++)
    {
      if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
      else dimmer = random8(1,3);           // return strokes are brighter than the leader
      
      fill_solid(leds4,NUM_LEDSshort,CHSV(160, 255, 255/dimmer));
      fill_solid(leds6,NUM_LEDSshort,CHSV(160, 255, 255/dimmer));

      FastLED.show();
      delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds

      fill_solid(leds4,NUM_LEDSshort,CHSV(0, 0, 0));
      fill_solid(leds6,NUM_LEDSshort,CHSV(0, 0, 0));
      FastLED.show();
      
      if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
      delay(50+random8(100));               // shorter delay between strokes  
    }
    delay(random8(FREQUENCY)*100);          // delay between strikes
}


/////////////////////////////////////////////
// METEOR SHOWER 
// 5/27/2018
// Fast zooming meteors
/////////////////////////////////////////////

uint8_t Longhue = 120;
byte Longidex = 255;
byte LongmeteorLength = 8;
int LongbyteCount = 155;
byte Longtemp[3];
  
void meteorShowerLong(){

    // start by fading to black
    //fadeToBlackBy( leds, NUM_LEDS, 20);
   //for(int i=0; i<100; i++){
  // slide all the pixels down one in the array
   memmove8( &leds3[1], &leds3[0], ((NUM_LEDSlong - 1) * 3) );
   memmove8( &leds5[1], &leds5[0], ((NUM_LEDSlong - 1) * 3) );

  // increment the meteor display frame
  Longidex++;
  // make sure we don't drift into space
  if ( Longidex > LongmeteorLength ) {
    Longidex = 255;
    // cycle through hues in each successive meteor tail
    Longhue+=1;
    //hue = 90;  
  }

  // this switch controls the actual meteor animation, i.e., what gets placed in the
  // first position and then subsequently gets moved down the strip by the memmove above
  switch ( Longidex ) {
  case 0:
    leds3[0] = CRGB(200,200,200);
    leds5[0] = CRGB(200,200,200); 

    break;
  case 1:
    leds3[0] = CHSV((Longhue), 255, 210);
    leds5[0] = CHSV((Longhue), 255, 210);
    break;
  case 2:
    leds3[0] = CHSV((Longhue), 255, 200); 
    leds5[0] = CHSV((Longhue), 255, 200); 
    break;
  case 3:
    leds3[0] = CHSV((Longhue), 255, 190); 
    leds5[0] = CHSV((Longhue), 255, 190); 
    break;
  case 4:
    leds3[0] = CHSV((Longhue), 255, 180); 
    leds5[0] = CHSV((Longhue), 255, 180); 

    break;
  case 5:
    leds3[0] = CHSV((Longhue), 255, 170); 
    leds5[0] = CHSV((Longhue), 255, 170); 
    break;
  case 6:
    leds3[0] = CHSV((Longhue), 160, 160);
    leds5[0] = CHSV((Longhue), 160, 160);
    break;
  case 7:
    leds3[0] = CHSV((Longhue), 140, 170);
    leds5[0] = CHSV((Longhue), 140, 170);
    break;
  case 8:
    leds3[0] = CHSV((Longhue), 120, 160); 
    leds5[0] = CHSV((Longhue), 120, 160); 
    break;
  case 9:
    leds3[0] = CHSV((Longhue), 100, 150); 
    leds5[0] = CHSV((Longhue), 100, 150);
    break;
  case 10:
    leds3[0] = CHSV((Longhue), 100, 140); 
    leds5[0] = CHSV((Longhue), 100, 140);
    break;
  case 11:
    leds3[0] = CHSV((Longhue), 100, 150);
    leds5[0] = CHSV((Longhue), 100, 150);
    break;
  case 12:
    leds3[0] = CHSV((Longhue ), 100, 150); 
    leds5[0] = CHSV((Longhue ), 100, 150);
    break;
  case 13:
    leds3[0] = CHSV(Longhue, 100, 150);
    leds5[0] = CHSV(Longhue, 100, 150);
    break;
  case 14:
    leds3[0] = CHSV((Longhue ), 100, 90); 
    leds5[0] = CHSV((Longhue ), 100, 90);
    break;
  case 15:
    leds3[0] = CHSV((Longhue ), 100, 80); 
    leds5[0] = CHSV((Longhue ), 100, 80); 
    break;
  case 16:
    leds3[0] = CHSV((Longhue ), 100, 70); 
    leds5[0] = CHSV((Longhue ), 100, 70);
    break;
  case 17:
    leds3[0] = CHSV((Longhue ), 100, 60); 
    leds5[0] = CHSV((Longhue ), 100, 60); 
    break;
  case 18:
    leds3[0] = CHSV((Longhue ), 100, 50); 
    leds5[0] = CHSV((Longhue ), 100, 50);
    break;
  default:
    leds3[0] = CRGB::Black; 
    leds5[0] = CRGB::Black; 
  }

  // show the blinky
  FastLED.show();  
  // control the animation speed/frame rate
  delay(20);
     //}
  
}

/////////////////////////////////////////////
// METEOR SHOWER 
// 5/27/2018
// Fast zooming meteors
/////////////////////////////////////////////

uint8_t Shorthue = 50;
byte Shortidex = 255;
byte meteorLengthShort = 12;
int ShortbyteCount = 155;
byte Shorttemp[3];
  
void meteorShowerShort(){

    // start by fading to black
    //fadeToBlackBy( leds, NUM_LEDS, 20);
   //for(int i=0; i<100; i++){
  // slide all the pixels down one in the array

  memmove8( &leds4[1], &leds4[0], ((NUM_LEDSshort - 1) * 3) );
  memmove8( &leds6[1], &leds6[0], ((NUM_LEDSshort - 1) * 3) );



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
    leds4[0] = CRGB(200,200,200); 
    leds6[0] = CRGB(200,200,200); 

    break;
  case 1:
    leds4[0] = CHSV((Shorthue), 255, 210);
    leds6[0] = CHSV((Shorthue), 255, 210); 
    break;
  case 2:
    leds4[0] = CHSV((Shorthue), 255, 200); 
    leds6[0] = CHSV((Shorthue), 255, 200); 
    break;
  case 3:
    leds4[0] = CHSV((Shorthue), 255, 190); 
    leds6[0] = CHSV((Shorthue), 255, 190); 
    break;
  case 4:
    leds4[0] = CHSV((Shorthue), 255, 180); 
    leds6[0] = CHSV((Shorthue), 255, 180); 

    break;
  case 5:
    leds4[0] = CHSV((Shorthue), 255, 170); 
    leds6[0] = CHSV((Shorthue), 255, 170); 
    break;
  case 6:
    leds4[0] = CHSV((Shorthue), 160, 160);
    leds6[0] = CHSV((Shorthue), 160, 160);
    break;
  case 7:
    leds4[0] = CHSV((Shorthue), 140, 170);
    leds6[0] = CHSV((Shorthue), 140, 170); 
    break;
  case 8:
    leds4[0] = CHSV((Shorthue), 120, 160); 
    leds6[0] = CHSV((Shorthue), 120, 160); 
    break;
  case 9:
    leds4[0] = CHSV((Shorthue), 100, 150);
    leds6[0] = CHSV((Shorthue), 100, 150);
    break;
  case 10:
    leds4[0] = CHSV((Shorthue), 100, 140);
    leds6[0] = CHSV((Shorthue), 100, 140);
    break;
  case 11:
    leds4[0] = CHSV((Shorthue), 100, 150);
    leds6[0] = CHSV((Shorthue), 100, 150); 
    break;
  case 12:
    leds4[0] = CHSV((Shorthue ), 100, 150);
    leds6[0] = CHSV((Shorthue ), 100, 150);
    break;
  
  default:
    leds4[0] = CRGB::Black; 
    leds6[0] = CRGB::Black; 
  }

  // show the blinky
  FastLED.show();  
  // control the animation speed/frame rate
  delay(20);
     //}
  
}
