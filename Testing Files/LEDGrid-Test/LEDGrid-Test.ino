#include <FastLED.h>

#define LED_PIN     17
#define NUM_LEDS    440 // 21 LEDs per row * 6 rows
#define BRIGHTNESS  64  // Adjust brightness as needed
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  for(int i = 0; i < NUM_LEDS; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Black); 
    leds[i] = CRGB::White; 
    FastLED.show();
  
  }
}
