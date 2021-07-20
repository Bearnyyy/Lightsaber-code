/*
Simple LED lightsaber code
Made by Bearny (https://twitter.com/bearnyyy)
Special thanks to u/johnny5canuck

Features:
  *Turn on and off effect
  *Multiple expressions with one button
  *3 modes:
    *Solid color with breathing/force effect 
    *Shimmer
    *Rainbow
    
Known issues:
  *Lightsaber can only be turned off while being in the first mode (solid color)
*/
#include <FastLED.h>
#include <OneButton.h>         

#define NUM_LEDS  112
#define LED_DT   13
#define BTN_PIN   4

uint16_t num_leds = 0;
bool leddir = 0;
 
uint8_t max_bright = 255;
 
struct CRGB leds[NUM_LEDS];
 
uint8_t color = 32;
uint8_t hue = 0;
uint8_t patternCounter = 0;
 
OneButton btn = OneButton(BTN_PIN, true);
 
CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;
 
void setup() {
 
  Serial.begin(115200);
  LEDS.addLeds<WS2812, LED_DT, GRB>(leds, NUM_LEDS);
  currentBlending = LINEARBLEND;
 
  uint8_t shimcol = random8();
  currentPalette = CRGBPalette16(CHSV(shimcol, 224, random8(64, 255)), CHSV(shimcol + 3, 240, random8(64, 255)), CHSV(shimcol, 192, random8(224, 255)), CHSV(shimcol - 3, 240, random8(224, 255)));
 
  FastLED.setBrightness(max_bright);
  FastLED.setCorrection(TypicalPixelString);
 
  btn.attachLongPressStart(onOff); //changes modes 
  btn.attachClick(nextColor); //changes the base color
  btn.attachDoubleClick(nextPattern); //turn on/off 
} 

void loop () {
 
  switch (patternCounter) {
    case 0:
      baseColor();
      break;
    case 1:
      shimmer();
      break;
    case 2:
      rainbow();
      break;
  }
 
  EVERY_N_MILLIS(8) {
    if (leddir) {
      if (num_leds < NUM_LEDS / 2) num_leds++;
    } else {
      if (num_leds > 0) num_leds--;
    }
  }

  if (!leddir) {

    uint16_t ranHigh = random16(NUM_LEDS / 2 - num_leds);
    uint16_t ranLow = random16(NUM_LEDS / 2 - num_leds);
    fadeToBlackBy(leds + NUM_LEDS / 2 + ranHigh, 1, 175);     // Fade the area between num_leds+NUM_LEDS/2 and NUM_LEDS.
    fadeToBlackBy(leds + num_leds + ranLow, 1, 175);                                            // Fade the area between num_leds and NUM_LEDS/2.
  }

  FastLED.show();
  btn.tick();
 
} 
 
void onOff() {
  leddir = !leddir;
}
 
void nextPattern() {
  patternCounter = (patternCounter + 1) % 3;
}
 
void nextColor() {
  color = color + 32;
  uint8_t shimcol = random8();
  currentPalette = CRGBPalette16(CHSV(shimcol, 224, random8(64, 255)), CHSV(shimcol + 3, 240, random8(64, 255)), CHSV(shimcol, 192, random8(224, 255)), CHSV(shimcol - 3, 240, random8(224, 255)));
}
 
void baseColor() { 
  uint8_t oneBeat = beatsin8(30, 200, 255, 0, 0); 
  uint8_t twoBeat = beatsin8(60, 200, 255, 0, 0); 
  uint8_t treeBeat = beatsin8(90, 200, 255, 0, 0); 
  uint8_t sinBeat = (oneBeat + twoBeat + treeBeat) / 3;
 
  fill_solid(leds, num_leds, CHSV(color, 255, sinBeat));                         
  fill_solid(leds + NUM_LEDS - num_leds, num_leds, CHSV(color, 255, sinBeat)); 
}
 
void rainbow()  { 
    uint8_t oneBeat = beatsin8(30, 200, 255, 0, 0); 
    uint8_t twoBeat = beatsin8(60, 200, 255, 0, 0); 
    uint8_t treeBeat = beatsin8(90, 200, 255, 0, 0); 
      uint8_t sinBeat = (oneBeat + twoBeat + treeBeat)/3;  
      
  for (int i = NUM_LEDS/2; i < NUM_LEDS; i++) {  
    leds[NUM_LEDS - i] = CHSV(hue + (i * 5), 255, sinBeat);
 }
   for (int j = NUM_LEDS/2; j < NUM_LEDS; j++) { 
    leds[j] = CHSV(hue + (j * 5), 255, sinBeat);
 }    
  EVERY_N_MILLISECONDS(8){
    hue++;
  } 
}
 
void shimmer() {                                                  
 
  static int16_t dist;                                     
  uint16_t xscale = 35;                                     
  uint16_t yscale = 30;                                         
 
  for (int i = NUM_LEDS / 2; i < NUM_LEDS; i++) {                                  
   uint8_t index = inoise8(i * xscale, dist + i * yscale) % 255;          
   leds[ (NUM_LEDS) - i ] = CHSV(color, 255, index);
  }
  for (int j = NUM_LEDS / 2; j < NUM_LEDS; j++) {                                    
    uint8_t index = inoise8(j * xscale, dist + j * yscale) % 255;         
    leds[j] = CHSV(color, 255, index);
  }
  dist += inoise8(millis(), millis()) / 24;
}
