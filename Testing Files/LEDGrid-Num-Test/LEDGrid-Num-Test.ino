#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>

#define PIN 17
#define MATRIX_WIDTH 22
#define MATRIX_HEIGHT 20
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

void setup() {
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);
  Serial.begin(9600); // Start serial communication at 9600 baud
}

// Variables for puck movement
int puckX = MATRIX_WIDTH / 2;
int puckY = MATRIX_HEIGHT / 2;
int puckDirX = 1;
int puckDirY = 1;

void loop() {
  matrix->fillScreen(0); // Clear the matrix

  // Draw the table boundaries
  matrix->drawRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, matrix->Color(255, 255, 255));

  // Draw the center line
  matrix->drawLine(MATRIX_WIDTH / 2, 0, MATRIX_WIDTH / 2, MATRIX_HEIGHT, matrix->Color(255, 255, 255));

  // Update and draw the puck position
  puckX += puckDirX;
  puckY += puckDirY;

  // Check for collisions with the table boundaries and change direction
  if (puckX <= 1 || puckX >= MATRIX_WIDTH - 2) puckDirX = -puckDirX;
  if (puckY <= 1 || puckY >= MATRIX_HEIGHT - 2) puckDirY = -puckDirY;

  // Draw the puck
  matrix->fillCircle(puckX, puckY, 1, matrix->Color(255, 0, 0)); // Puck as a small red circle

  matrix->show();
  delay(20); // Adjust for puck speed
}
