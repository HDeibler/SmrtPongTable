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
  FastLED.addLeds<WS2812B, PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // Adjusted color order to RGB
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(BRIGHTNESS);
}

void loop() {
  drawAmericanFlag();
  matrix->show();
  // No need to update continuously
  delay(10000); // Delay for a while to keep the flag displayed
}

void drawAmericanFlag() {
    matrix->fillScreen(0); // Clear the matrix
    
    // Convert CRGB colors to uint32_t explicitly if necessary
    uint32_t blue = matrix->Color(0, 0, 255);
    uint32_t white = matrix->Color(255, 255, 255);
    uint32_t red = matrix->Color(255, 0, 0);
    
    // Draw stripes
    for(int y = 0; y < MATRIX_HEIGHT; y++) {
        // Alternating stripe colors
        for(int x = 0; x < MATRIX_WIDTH; x++) {
            if (y < 9 && x < 12) {
                // Draw the blue field
                matrix->drawPixel(x, y, blue);
            } else {
                // Draw stripes, alternating colors
                uint32_t color = (y % 2 == 0) ? red : white;
                matrix->drawPixel(x, y, color);
            }
        }
    }
    
    // Simplified stars in the blue field
    for(int starY = 1; starY < 9; starY += 2) {
        for(int starX = 1; starX < 12; starX += 3) {
            matrix->drawPixel(starX, starY, white);
        }
    }
    
    matrix->show();
}
