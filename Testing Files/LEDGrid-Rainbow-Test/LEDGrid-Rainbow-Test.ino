#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>

#define PIN 17
#define MATRIX_WIDTH 22
#define MATRIX_HEIGHT 20
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 40

CRGB leds[NUM_LEDS];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

void setup() {
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
}

void loop() {
  static uint8_t hue = 0;
  // Fill the matrix with a moving color wave
  for (int y = 0; y < MATRIX_HEIGHT; y++) {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
      // Calculate the hue based on the current position and time
      uint8_t calculatedHue = hue + (x * 10) + (y * 10);
      matrix->drawPixel(x, y, CHSV(calculatedHue, 255, 255));
    }
  }
  
  // Show the matrix
  matrix->show();
  
  // Increase the starting hue for the next frame to create the animation effect
  hue++;

  // Control the speed of the animation
  delay(1);
}
